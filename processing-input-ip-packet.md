# processing input IPv4 packet

Frame carrying  IP packet is handled by ip_rcv function, which is registered during kernel initialization

    static struct packet_type ip_packet_type = {
           .type = __constant_htons(ETH_P_IP),
           .func = ip_rcv,
           .gso_send_check = inet_gso_send_check,
           .gso_segment = inet_gso_segment,
    };
    
    static int __init inet_init(void)
    {
    …
           dev_add_pack(&ip_packet_type);
    
    
    void dev_add_pack(struct packet_type *pt)
    {
           int hash;
    
    
           spin_lock_bh(&ptype_lock);
           if (pt->type == htons(ETH_P_ALL)) {
                   netdev_nit++;
                   list_add_rcu(&pt->list, &ptype_all);
           } else {
                   hash = ntohs(pt->type) & 15;
                   list_add_rcu(&pt->list, &ptype_base[hash]);
           }
           spin_unlock_bh(&ptype_lock);
    }


`ip_packet_type` get registered in global variable `ptype_base`, which is then used in function

    int netif_receive_skb(struct sk_buff *skb)
    {
    ...
           type = skb->protocol;
          /* normal protocol handlers are registered in ptype_base */
           list_for_each_entry_rcu(ptype, &ptype_base[ntohs(type)&15], list) {
    …
                                  ret = deliver_skb(skb, pt_prev, orig_dev);
    

Due to presence of netfilter firewall component, the `ip_rcv` however just perform sanity check 
(e.g. ip version, checksum, drop frame to other host), then invoke netfilter hook if passed, `ip_rcv_finish` 
will perform the main of work (decide of local delivery vs. forward, parse ip options).

    int ip_rcv(struct sk_buff *skb, struct net_device *dev, struct packet_type *pt, struct net_device *orig_dev)
    {
           struct iphdr *iph;
           u32 len;
    
    
           /* When the interface is in promisc. mode, drop all the crap
            * that it receives, do not try to analyse it.
            */
           if (skb->pkt_type == PACKET_OTHERHOST)
                   goto drop;
    
    
           IP_INC_STATS_BH(IPSTATS_MIB_INRECEIVES);
    
    
           if ((skb = skb_share_check(skb, GFP_ATOMIC)) == NULL) {
                   IP_INC_STATS_BH(IPSTATS_MIB_INDISCARDS);
                   goto out;
           }
    
    
           if (!pskb_may_pull(skb, sizeof(struct iphdr)))
                   goto inhdr_error;
    
    
           iph = skb->nh.iph;
    
    
        /*
            *      RFC1122: 3.1.2.2 MUST silently discard any IP frame that fails the checksum.
            *
            *      Is the datagram acceptable?
            *
            *      1.      Length at least the size of an ip header
            *      2.      Version of 4
            *      3.      Checksums correctly. [Speed optimisation for later, skip loopback checksums]
            *      4.      Doesn't have a bogus length
            */
    
    
           if (iph->ihl < 5 || iph->version != 4)
                   goto inhdr_error;
    
    
           if (!pskb_may_pull(skb, iph->ihl*4))
                   goto inhdr_error;
    
    
           iph = skb->nh.iph;
    
    
           if (unlikely(ip_fast_csum((u8 *)iph, iph->ihl)))
                   goto inhdr_error;
    
    
           len = ntohs(iph->tot_len);
           if (skb->len < len) {
                   IP_INC_STATS_BH(IPSTATS_MIB_INTRUNCATEDPKTS);
                   goto drop;
           } else if (len < (iph->ihl*4))
                   goto inhdr_error;
    
    
           /* Our transport medium may have padded the buffer out. Now we know it
            * is IP we can trim to the true length of the frame.
            * Note this now means skb->len holds ntohs(iph->tot_len).
            */
           if (pskb_trim_rcsum(skb, len)) {
                   IP_INC_STATS_BH(IPSTATS_MIB_INDISCARDS);
                   goto drop;
           }
    
    
           /* Remove any debris in the socket control block */
           memset(IPCB(skb), 0, sizeof(struct inet_skb_parm));
    
    
           return NF_HOOK(PF_INET, NF_IP_PRE_ROUTING, skb, dev, NULL,
                          ip_rcv_finish);
    
    
    inhdr_error:
           IP_INC_STATS_BH(IPSTATS_MIB_INHDRERRORS);
    drop:
           kfree_skb(skb);
    out:
           return NET_RX_DROP;
    }
    
    
    static inline int ip_rcv_finish(struct sk_buff *skb)
    {
           struct iphdr *iph = skb->nh.iph;
           struct rtable *rt;
    
    
           /*
            *      Initialise the virtual path cache for the packet. It describes
            *      how the packet travels inside Linux networking.
            */
           if (skb->dst == NULL) {
                   int err = ip_route_input(skb, iph->daddr, iph->saddr, iph->tos,
                                            skb->dev);
                   if (unlikely(err)) {
                           if (err == -EHOSTUNREACH)
                                   IP_INC_STATS_BH(IPSTATS_MIB_INADDRERRORS);
                           else if (err == -ENETUNREACH)
                                   IP_INC_STATS_BH(IPSTATS_MIB_INNOROUTES);
                           goto drop;
                   }
           }
    
    
    #ifdef CONFIG_NET_CLS_ROUTE
           if (unlikely(skb->dst->tclassid)) {
                   struct ip_rt_acct *st = ip_rt_acct + 256*smp_processor_id();
                   u32 idx = skb->dst->tclassid;
                   st[idx&0xFF].o_packets++;
                   st[idx&0xFF].o_bytes+=skb->len;
                   st[(idx>>16)&0xFF].i_packets++;
                   st[(idx>>16)&0xFF].i_bytes+=skb->len;
           }
    #endif
    
    
           if (iph->ihl > 5 && ip_rcv_options(skb))
                   goto drop;
    
    
           rt = (struct rtable*)skb->dst;
           if (rt->rt_type == RTN_MULTICAST)
                   IP_INC_STATS_BH(IPSTATS_MIB_INMCASTPKTS);
           else if (rt->rt_type == RTN_BROADCAST)
                   IP_INC_STATS_BH(IPSTATS_MIB_INBCASTPKTS);
    
    
           return dst_input(skb);
    
    
    drop:
           kfree_skb(skb);
           return NET_RX_DROP;
    }
    
The function ip_route_input intefaces with routing system to decide of whether local delivery or forward

    int ip_route_input(struct sk_buff *skb, u32 daddr, u32 saddr,
                      u8 tos, struct net_device *dev)
    {
    …
          return ip_route_input_slow(skb, daddr, saddr, tos, dev);
    }
    

    static int ip_route_input_slow(struct sk_buff *skb, u32 daddr, u32 saddr,
                                  u8 tos, struct net_device *dev)
    {
           struct fib_result res;
           struct in_device *in_dev = in_dev_get(dev);
    
    
    …
           err = ip_mkroute_input(skb, &res, &fl, in_dev, daddr, saddr, tos); /* this path leads to packet forward */
    ...
    
    
    local_input:
           rth = dst_alloc(&ipv4_dst_ops);
           if (!rth)
    
    
    …
           rth->u.dst.input= ip_local_deliver; /* assign function for local deliver the ip packet */
    
    
    
    
    static inline int ip_mkroute_input(struct sk_buff *skb,
                                      struct fib_result* res,
                                      const struct flowi *fl,
                                      struct in_device *in_dev,
                                      u32 daddr, u32 saddr, u32 tos)
    …
                   err = __mkroute_input(skb, res, in_dev, daddr, saddr, tos,
                                         &rth);
    ...
    
    
    
    
    static inline int __mkroute_input(struct sk_buff *skb,
                                     struct fib_result* res,
                                     struct in_device *in_dev,
                                     u32 daddr, u32 saddr, u32 tos,
                                     struct rtable **result)
    …
           rth->u.dst.input = ip_forward; /* assign function for forward ip packet*/
    ...
    

The `ip_local_delivery` delivers packet to L4 protocol handler, which is registered in global variable `inet_protos`

    struct net_protocol *inet_protos[MAX_INET_PROTOS];
    
    
    int inet_add_protocol(struct net_protocol *prot, unsigned char protocol)
    {
           int hash, ret;
    
    
           hash = protocol & (MAX_INET_PROTOS - 1);
    
    
           spin_lock_bh(&inet_proto_lock);
           if (inet_protos[hash]) {
                   ret = -1;
           } else {
                   inet_protos[hash] = prot;
                   ret = 0;
           }
           spin_unlock_bh(&inet_proto_lock);
    
    
           return ret;
    }
    
    
    static struct net_protocol icmp_protocol = {
    .handler = icmp_rcv, };
    
    
    static int __init inet_init(void)
    {
    …
           if (inet_add_protocol(&icmp_protocol, IPPROTO_ICMP) < 0)
                   printk(KERN_CRIT "inet_init: Cannot add ICMP protocol\n");
    
    
The `ip_local_deliver` use netfilter hook to check firewall rule before calling `ip_local_delivery_finish`
    
    
    int ip_local_deliver(struct sk_buff *skb)
    {
           /*
            *      Reassemble IP fragments.
            */
    
    
           if (skb->nh.iph->frag_off & htons(IP_MF|IP_OFFSET)) {
                   skb = ip_defrag(skb, IP_DEFRAG_LOCAL_DELIVER);
                   if (!skb)
                           return 0;
           }
    
    
           return NF_HOOK(PF_INET, NF_IP_LOCAL_IN, skb, skb->dev, NULL,
                          ip_local_deliver_finish);
    }
    

`ip_local_deliver_finish` find L4 protocol in L3 protocol (ip) header

    static inline int ip_local_deliver_finish(struct sk_buff *skb)
    {
    …
                   int protocol = skb->nh.iph->protocol;
    …
                   hash = protocol & (MAX_INET_PROTOS - 1);
    …
                   if ((ipprot = rcu_dereference(inet_protos[hash])) != NULL) {
                           int ret;
    
    
                           if (!ipprot->no_policy) {
                                   if (!xfrm4_policy_check(NULL, XFRM_POLICY_IN, skb)) {
                                           kfree_skb(skb);
                                           goto out;
                                   }
                                   nf_reset(skb);
                           }
                           ret = ipprot->handler(skb); /*invoke L4 protocol handler */
                           if (ret < 0) {
                                   protocol = -ret;
                                   goto resubmit;
                           }
                           IP_INC_STATS_BH(IPSTATS_MIB_INDELIVERS);
    
    
    static struct net_protocol udp_protocol = {
           .handler =      udp_rcv,
           .err_handler =  udp_err,
           .no_policy =    1,
    };
    
    
    static int __init inet_init(void)
    {
    …
    if (inet_add_protocol(&udp_protocol, IPPROTO_UDP) < 0)
                   printk(KERN_CRIT "inet_init: Cannot add UDP protocol\n");
    ...
    
    
    int udp_rcv(struct sk_buff *skb)
    {
    …
           struct sock *sk; /* internal representation of socket*/
    
    
    ...   
           sk = udp_v4_lookup(saddr, uh->source, daddr, uh->dest, skb->dev->ifindex);
    
    
           if (sk != NULL) {
                   int ret = udp_queue_rcv_skb(sk, skb);
                   sock_put(sk);
    
    
    static __inline__ struct sock *udp_v4_lookup(u32 saddr, u16 sport,
                                                u32 daddr, u16 dport, int dif)
    {
           struct sock *sk;
    
    
           read_lock(&udp_hash_lock);
           sk = udp_v4_lookup_longway(saddr, sport, daddr, dport, dif);
           if (sk)
                   sock_hold(sk);
           read_unlock(&udp_hash_lock);
           return sk;
    }
    

This method try to find sock that satisfies condition of the packet in term of source , destination address and port


    static struct sock *udp_v4_lookup_longway(u32 saddr, u16 sport,
                                             u32 daddr, u16 dport, int dif)
    {
           struct sock *sk, *result = NULL;
    ...
    
    
           sk_for_each(sk, node, &udp_hash[hnum & (UDP_HTABLE_SIZE - 1)]) {
                   struct inet_sock *inet = inet_sk(sk);
    …

The method that place packet to socket queue for user space program 

    static int udp_queue_rcv_skb(struct sock * sk, struct sk_buff *skb)
    {
    …
           struct udp_sock *up = udp_sk(sk);
    …
            if (!sock_owned_by_user(sk))
                    rc = __udp_queue_rcv_skb(sk, skb);
            else
                    sk_add_backlog(sk, skb);
    
    
    static int __udp_queue_rcv_skb(struct sock *sk, struct sk_buff *skb)
    {
            int rc;
    
    
            if ((rc = sock_queue_rcv_skb(sk, skb)) < 0) {
                    /* Note that an ENOMEM error is charged twice */
                    if (rc == -ENOMEM)
                            UDP_INC_STATS_BH(UDP_MIB_INERRORS);
                    goto drop;
            }
           …
    
          int sock_queue_rcv_skb(struct sock *sk, struct sk_buff *skb)
          {
          …
                  skb_queue_tail(&sk->sk_receive_queue, skb);
                 if (!sock_flag(sk, SOCK_DEAD))
                          sk->sk_data_ready(sk, skb_len);
          
The sk_data_ready is initialized when creating socket and its internal representation

      struct sock {
      ...
              void                    (*sk_data_ready)(struct sock *sk, int bytes);
      ...
      
      
      void sock_init_data(struct socket *sock, struct sock *sk)
      {
      ...
             sk->sk_state_change     =       sock_def_wakeup;
             sk->sk_data_ready       =       sock_def_readable;
      
      
      

This function wake up processes sleeping in socket queue

      static void sock_def_readable(struct sock *sk, int len)
      {
              read_lock(&sk->sk_callback_lock);
              if (sk_has_sleeper(sk))
                      wake_up_interruptible(sk->sk_sleep);
              sk_wake_async(sk,1,POLL_IN);
              read_unlock(&sk->sk_callback_lock);
      }
      
References

1. http://www.cyberciti.biz/files/linux-kernel/Documentation/networking/ip-sysctl.txt
