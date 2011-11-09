# Receive frame from network


The driver code need to be written in such way to ensure minimize frame’s lost at the same time giving CPU to other processes under high network load.
There are two techniques used by kernel to know about incoming frame a) interrupt and b) polling. While polling (when timer expires) is non efficient under low traffic, interrupt is non efficient under high traffic.
Linux kernel NAPI - New API combines both techniques to archive low latency and fairness.  However there are still many drivers that use old API employing only interrupt technique.


When a frame is received in NIC, it raises interrupt that is delivered to one of CPU. With old API, the interrupt handler (implemented by driver e.g. vortex_rx) then transfer the frame from NIC buffer placing it into host memory by calling netif_rx.  


static irqreturn_t
vortex_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
…
               if (status & RxComplete)
                       vortex_rx(dev);
...


static int vortex_rx(struct net_device *dev)
{
…
                      skb = dev_alloc_skb(pkt_len + 5);
…
                               skb->protocol = eth_type_trans(skb, dev); /* figure out protocol handler */
                               netif_rx(skb);
...


int netif_rx(struct sk_buff *skb)
{
...
       queue = &__get_cpu_var(softnet_data);
...
enqueue:
                       dev_hold(skb->dev);
                       __skb_queue_tail(&queue->input_pkt_queue, skb);
...
               netif_rx_schedule(&queue->backlog_dev); /* this is fake dev*/


The netif_rx adds frame to per CPU input queue softnet_data->input_pkt_queue and call netif_rx_schedule to put a signal for bottom haft netif_rx_action by adding a fake backlog_dev device in to softnet_data->poll_list.


void __netif_rx_schedule(struct net_device *dev)
{
       unsigned long flags;


       local_irq_save(flags);
       dev_hold(dev);
       list_add_tail(&dev->poll_list, &__get_cpu_var(softnet_data).poll_list);
       if (dev->quota < 0)
               dev->quota += dev->weight;
       else
               dev->quota = dev->weight;
       __raise_softirq_irqoff(NET_RX_SOFTIRQ); /* trigger net_rx_action */
       local_irq_restore(flags);
}


The list structure in kernel  is kind of embedded container inside data structure, so adding &dev->poll_list into softnet_data->poll_list means add the dev into the list.


Example of NAPI is tg3_rx of broadcom driver, which copy frame from NIC to the input queue managed by the driver itself.


static irqreturn_t tg3_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
…
               netif_rx_schedule(tnapi->dummy_netdev);
...


static int tg3_rx(struct tg3_napi *tnapi, int budget)
{
…
                       netif_rx_schedule(tp->napi[1].dummy_netdev);


static int tg3_poll(struct net_device *netdev, int *budget)
{
...
               work_done = tg3_rx(tnapi, orig_budget);
...


The softnet_data->poll_list seems does not contain a list of devices that has some incoming frames waiting for process but just a fake device with sufficient information for net_rx_action to know how to poll frame from input queue.


In the old API, there is one input queue per CPU, its size is specified in parameter. The kernel drops incoming frame  if  the queue reaches its size.


[root@localhost tap]# cat /proc/sys/net/core/netdev_max_backlog
1000


In the NAPI input queue is per device and it is up to the driver to handle it.


The top haft is a function of a specific driver while the bottom haft is always kernel function net_rx_action.  The net_rx_action get the frames either from softnet_data->input_pkt_queue or by calling NAPI driver net_device->poll.  


The default net_device->poll is set to process_backlog
static int __init net_dev_init(void)
{
…
       for_each_possible_cpu(i) {
               struct softnet_data *queue;
...
               queue->backlog_dev.poll = process_backlog;


This generic backlog_dev  was put into softnet_data->poll_list in netif_rx in previous stage. The net_rx_action check poll_list of devices and invoke poll of each device


static void net_rx_action(struct softirq_action *h)
{
…
       while (!list_empty(&queue->poll_list)) {
...
               dev = list_entry(queue->poll_list.next,
                                struct net_device, poll_list);
…
                       poll_result = dev->poll(dev, &budget);


The process_backlog used to dequeue frame for old API driver and send it up to protocol handler by calling netif_receive_skb


static int process_backlog(struct net_device *backlog_dev, int *budget)
{
...
       for (;;) {
...
               skb = __skb_dequeue(&queue->input_pkt_queue);
...
               netif_receive_skb(skb);


In the NAPI tg3 driver of broadcom NIC, the net_device->poll is set to tg3_poll


static int __devinit tg3_init_one(struct pci_dev *pdev,
                                 const struct pci_device_id *ent)
{
…
      dev->poll = tg3_poll;


static int tg3_poll(struct net_device *netdev, int *budget)
{
...
                work_done = tg3_rx(tnapi, orig_budget);


static int tg3_rx(struct tg3_napi *tnapi, int budget)
{
…
                       netif_receive_skb(skb);


The netif_receive_skb then deliver the frame to tap and protocol handler


int netif_receive_skb(struct sk_buff *skb)
{
…
       /* protocol sniffers - ETH_P_ALL  are registered in ptype_all */
       list_for_each_entry_rcu(ptype, &ptype_all, list) {
...
                               ret = deliver_skb(skb, pt_prev, orig_dev);
…


       type = skb->protocol;
      /* normal protocol handlers are registered in ptype_base */
       list_for_each_entry_rcu(ptype, &ptype_base[ntohs(type)&15], list) {
…
                              ret = deliver_skb(skb, pt_prev, orig_dev);
...




static __inline__ int deliver_skb(struct sk_buff *skb,
                                 struct packet_type *pt_prev,
                                 struct net_device *orig_dev)
{
       atomic_inc(&skb->users);
       return pt_prev->func(skb, skb->dev, pt_prev, orig_dev);
}




References
1. http://en.wikipedia.org/wiki/New_API
2. http://knol.google.com/k/napi-linux-new-api#
3. http://www.redhat.com/promo/summit/2008/downloads/pdf/Thursday/Mark_Wagner.pdf
4. http://www.cs.clemson.edu/~westall/853/notes/devrecv.pdf
5. http://isis.poly.edu/kulesh/stuff/src/klist/