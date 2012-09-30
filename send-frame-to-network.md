# Send a frame to network


Upper layer put output frame into driver buffer struct Qdisc *q = dev->qdisc using dev_queue_xmit


int dev_queue_xmit(struct sk_buff *skb)
{
       struct net_device *dev = skb->dev;


…
       q = rcu_dereference(dev->qdisc);
#ifdef CONFIG_NET_CLS_ACT
       skb->tc_verd = SET_TC_AT(skb->tc_verd,AT_EGRESS);
#endif
       if (q->enqueue) {
               /* Grab device queue */
               spin_lock(&dev->queue_lock);
               q = dev->qdisc;
               if (q->enqueue) {
                       rc = q->enqueue(skb, q);
                       qdisc_run(dev);
                       spin_unlock(&dev->queue_lock);


                       rc = rc == NET_XMIT_BYPASS ? NET_XMIT_SUCCESS : rc;
                       goto out;
               }
               spin_unlock(&dev->queue_lock);
       }


then call qdisc_run to send frames to the NIC.


static inline void qdisc_run(struct net_device *dev) {
while (!netif_queue_stopped(dev) && qdisc_restart(dev) < 0)
/* NOTHING */;
}


Due to various reasons (queue is stopped because of not enough memory of NIC, someone else is transfering), the immediate transfer may not success and have to be postponed to later time.


int qdisc_restart(struct net_device *dev) {


       if (!nolock) {
             if (!netif_tx_trylock(dev)) {
                collision:
                /* So, someone grabbed the driver. */
…
                     goto requeue;
              }
        }
...
requeue:
        if (skb->next)
             dev->gso_skb = skb;
        else
             q->ops->requeue(skb, q);
        netif_schedule(dev); /* schedule a transfer for later time */
        return 1;
}


The soft interrupt net_tx_action and netif_schedule are used to facilitate that.    


static inline void _ _netif_schedule(struct net_device *dev) {
if (!test_and_set_bit(_ _LINK_STATE_SCHED, &dev->state)) {
unsigned long flags;
struct softnet_data *sd;  
local_irq_save(flags);
sd = &_ _get_cpu_var(softnet_data);
dev->next_sched = sd->output_queue;
sd->output_queue = dev;
raise_softirq_irqoff(cpu, NET_TX_SOFTIRQ); /* trigger net_tx_action */
local_irq_restore(flags);
}
}


A soft interrupt net_tx_action is responsible for sending frame from driver buffer (kernel memory) to NIC.


static void net_tx_action(struct softirq_action *h)
{
...


if (sd->output_queue) {
struct net_device *head;   
local_irq_disable( );
head = sd->output_queue;
sd->output_queue = NULL;
local_irq_enable( );   
while (head) {
struct net_device *dev = head;
head = head->next_sched;   
smp_mb_ _before_clear_bit( );
clear_bit(_ _LINK_STATE_SCHED, &dev->state);   
if (spin_trylock(&dev->queue_lock)) {
qdisc_run(dev);
spin_unlock(&dev->queue_lock);
} else {
netif_schedule(dev);
}
}
}


int qdisc_restart(struct net_device *dev) {
struct Qdisc *q = dev->qdisc;
struct sk_buff *skb;   
if ((skb = q->dequeue(q)) != NULL) {
…
        if (!spin_trylock(&dev->xmit_lock)) {
        …
        }
{
if (!netif_queue_stopped(dev)) {
int ret;
if (netdev_nit)
dev_queue_xmit_nit(skb, dev);   /* send frame to protocol sniffer*/
ret = dev->hard_start_xmit(skb, dev); /* send frame to NIC */
if (ret == NETDEV_TX_OK) {
if (!nolock) {
dev->xmit_lock_owner = -1;
spin_unlock(&dev->xmit_lock);
}
spin_lock(&dev->queue_lock);
return -1;
}
if (ret == NETDEV_TX_LOCKED && nolock) {
spin_lock(&dev->queue_lock); goto collision;
}
                }
}


In dev->hard_start_xmit, when the device driver realizes that it does not have enough space to store a frame of maximum size (MTU), it stops the egress queue with netif_stop_queue to avoid wasting resources with future transmissions that the kernel already knows will fail.


The following example of this throttling at work is taken from vortex_start_xmit (the hard_start_xmit method used by the drivers/net/3c59x.c driver):


vortex_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
…
outsl(ioaddr + TX_FIFO, skb->data, (skb->len + 3) >> 2);
dev_kfree_skb (skb);
if (inw(ioaddr + TxFree) > 1536) {
netif_start_queue (dev); /* AKPM: redundant? */
} else {
        /* stop the queue */
netif_stop_queue(dev);
/* Interrupt us when the FIFO has room for max-sized packet. */
outw(SetTxThreshold + (1536>>2), ioaddr + EL3_CMD);
}


static void vortex_interrupt(int irq, void *dev_id, struct pt_regs *regs) {
…
if (status & TxAvailable) {
if (vortex_debug > 5)
printk(KERN_DEBUG " TX room bit was handled.\n");
/* There's room in the FIFO for a full-sized packet. */
outw(AckIntr | TxAvailable, ioaddr + EL3_CMD);
/* wake queue up to start transfer again when there is space in NIC*/
netif_wake_queue (dev);
}
…
}


static inline void netif_wake_queue(struct net_device *dev)
{
#ifdef CONFIG_NETPOLL_TRAP
       if (netpoll_trap())
               return;
#endif
       if (test_and_clear_bit(__LINK_STATE_XOFF, &dev->state))
               __netif_schedule(dev);
}