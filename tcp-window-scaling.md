# TCP Window Scaling if one of sides does not support window scaling

**Overview**

Every TCP packet includes, in the header, a "window" field which specifies how much data the system which sent the packet is willing and able to receive from the other end. 
The window is the flow control mechanism used by TCP; it controls the maximum amount of data which can be "in flight" between two communicating systems and keeps one side from overwhelming the other with data.
In order to increase receive window size of 16 bits, the TCP Window Scaling was introduced in RFC 1323. 

This is an option of TCP header. The scaling factor is 1 byte but standard mandates that we can use the value only up to 14.

        void tcp_parse_options(const struct sk_buff *skb, struct tcp_options_received *opt_rx,
                       const u8 **hvpp, int estab)
        {

           while (length > 0) {
           ...
                       case TCPOPT_WINDOW:
                                if (opsize == TCPOLEN_WINDOW && th->syn &&
                                    !estab && sysctl_tcp_window_scaling) {
                                        __u8 snd_wscale = *(__u8 *)ptr;
                                        opt_rx->wscale_ok = 1;
                                        if (snd_wscale > 14) {
                                                if (net_ratelimit())
                                                        pr_info("%s: Illegal window scaling value %d >14 received\n",
                                                                __func__,
                                                                snd_wscale);
                                                snd_wscale = 14;
                                        }
                                        opt_rx->snd_wscale = snd_wscale;
                                }
                                break;

In order to be less confusing we will use the following definitions

* Origininator - is the side that iniciates the communication which is usually a client
* Acceptor - is the side that accepts the request for establish the communication which is usually a server
* Receiver/Sender - is the side that receives/sends a segment, which can be the Originator or Acceptor depending on context as the communication is bi directional
            
This Window scale is sent when initializing TCP connection in SYN segment by client - the Originator and SYN/ACK by server - the Acceptor of the communication.

        21:20:05.370976 IP 192.168.1.133.50236 > www.creditotoronjadeingdirect.es.http: S 25373982:25373982(0) win 65535 <mss 1460,nop,wscale 3,nop,nop,timestamp 304748558 0,sackOK,eol>
        21:20:05.427365 IP www.creditotoronjadeingdirect.es.http > 192.168.1.133.50236: S 55794747:55794747(0) ack 25373983 win 14600 <mss 1452,wscale 0,eol>
        
After agree on window scaling factor in each direction. When sending window size to sender the receiver will right shift desire window 
size by value of scaling factor. When receving window size from sender the receiver will left shift desure window size by value of 
scaling factor.

If one of side does not support window scaling e.g `/proc/sys/net/ipv4/tcp_window_scaling` is `0` then window scaling must not be used
in neither sending nor receiving direction.

**The Acceptor**

When receiving SYN segment, the `tcp_rcv_state_process` is called 

        5837 *      This function implements the receiving procedure of RFC 793 for
        5838 *      all states except ESTABLISHED and TIME_WAIT.
        5839 *      It's called from both tcp_v4_rcv and tcp_v6_rcv and should be
        5840 *      address independent.
        5841 */
        5842
        5843int tcp_rcv_state_process(struct sock *sk, struct sk_buff *skb,
        5844                          const struct tcphdr *th, unsigned int len)
        5845{
        ...

        5892        case TCP_SYN_SENT:
        5893                queued = tcp_rcv_synsent_state_process(sk, skb, th, len);

Which in turn calls `tcp_rcv_synsent_state_process` 

        5566static int tcp_rcv_synsent_state_process(struct sock *sk, struct sk_buff *skb,
        5567                                         const struct tcphdr *th, unsigned int len)
        5568{
        ...
        5571        struct tcp_sock *tp = tcp_sk(sk);
        ...
        5575        tcp_parse_options(skb, &tp->rx_opt, &hash_location, 0);
        5576
        ...
        5577        if (th->ack) {
        ...

                        if (!tp->rx_opt.wscale_ok) {
        5647                        tp->rx_opt.snd_wscale = tp->rx_opt.rcv_wscale = 0;
        5648                        tp->window_clamp = min(tp->window_clamp, 65535U);
        5649                }
        5650
        ...
 
        5773        if (th->syn) {
        ...
        5804
        5805                tcp_send_synack(sk);

The `tcp_rcv_synsent_state_process` calls `tcp_parse_options` in first place, that check if there is window scaling option 
is present in the SYN segment and the `sys_tcp_window_scaling` is enable. If not so, then both send and receive window 
scaling is set to `0` in `rx_opt` of the socket

        3834void tcp_parse_options(const struct sk_buff *skb, struct tcp_options_received *opt_rx,
        3835                       const u8 **hvpp, int estab)
        3836{
        ...
        3844        while (length > 0) {
        ...
        3848                switch (opcode) {
        ...
        3872                        case TCPOPT_WINDOW:
        3873                                if (opsize == TCPOLEN_WINDOW && th->syn &&
        3874                                    !estab && sysctl_tcp_window_scaling) {
        3875                                        __u8 snd_wscale = *(__u8 *)ptr;
        3876                                        opt_rx->wscale_ok = 1;
        3877                                        if (snd_wscale > 14) {
        3878                                                if (net_ratelimit())
        3879                                                        pr_info("%s: Illegal window scaling value %d >14 received\n",
        3880                                                                __func__,
        3881                                                                snd_wscale);
        3882                                                snd_wscale = 14;
        3883                                        }
        3884                                        opt_rx->snd_wscale = snd_wscale;
        3885                                }
        3886                                break;


Now the `tcp_send_synack(sk)` is called to send SYN ack back to the sender. This function makes a call to `tcp_make_synack` to 
create SYN-ACK segment

        /* Prepare a SYN-ACK. */
        2430struct sk_buff *tcp_make_synack(struct sock *sk, struct dst_entry *dst,
        2431                                struct request_sock *req,
        2432                                struct request_values *rvp)
        2433{
        ...

        2461        if (req->rcv_wnd == 0) { /* ignored for retransmitted syns */
        2462                __u8 rcv_wscale;
        2463                /* Set this up on the first call only */
        2464                req->window_clamp = tp->window_clamp ? : dst_metric(dst, RTAX_WINDOW);
        2465
        2466                /* limit the window selection if the user enforce a smaller rx buffer */
        2467                if (sk->sk_userlocks & SOCK_RCVBUF_LOCK &&
        2468                    (req->window_clamp > tcp_full_space(sk) || req->window_clamp == 0))
        2469                        req->window_clamp = tcp_full_space(sk);
        2470
        2471                /* tcp_full_space because it is guaranteed to be the first packet */
        2472                tcp_select_initial_window(tcp_full_space(sk),
        2473                        mss - (ireq->tstamp_ok ? TCPOLEN_TSTAMP_ALIGNED : 0),
        2474                        &req->rcv_wnd,
        2475                        &req->window_clamp,
        2476                        ireq->wscale_ok,
        2477                        &rcv_wscale,
        2478                        dst_metric(dst, RTAX_INITRWND));
        2479                ireq->rcv_wscale = rcv_wscale;
        2480        }


The `tcp_make_synack` pass window scale option `ireq->wscale_ok` to `tcp_select_initial_window`, that decides the window scale to be sent
to the originator of tcp connection. It is `0` if the window scaling option is not enabled.

        189void tcp_select_initial_window(int __space, __u32 mss,
        190                               __u32 *rcv_wnd, __u32 *window_clamp,
        191                               int wscale_ok, __u8 *rcv_wscale,
        192                               __u32 init_rcv_wnd)
        193{
        194        unsigned int space = (__space < 0 ? 0 : __space);
        195
        196        /* If no clamp set the clamp to the max possible scaled window */
        197        if (*window_clamp == 0)
        198                (*window_clamp) = (65535 << 14);
        199        space = min(*window_clamp, space);
        200
        201        /* Quantize space offering to a multiple of mss if possible. */
        202        if (space > mss)
        203                space = (space / mss) * mss;
        204
        205        /* NOTE: offering an initial window larger than 32767
        206         * will break some buggy TCP stacks. If the admin tells us
        207         * it is likely we could be speaking with such a buggy stack
        208         * we will truncate our initial window offering to 32K-1
        209         * unless the remote has sent us a window scaling option,
        210         * which we interpret as a sign the remote TCP is not
        211         * misinterpreting the window field as a signed quantity.
        212         */
        213        if (sysctl_tcp_workaround_signed_windows)
        214                (*rcv_wnd) = min(space, MAX_TCP_WINDOW);
        215        else
        216                (*rcv_wnd) = space;
        217
        218        (*rcv_wscale) = 0;
        219        if (wscale_ok) {
        220                /* Set window scaling on max possible window
        221                 * See RFC1323 for an explanation of the limit to 14
        222                 */
        223                space = max_t(u32, sysctl_tcp_rmem[2], sysctl_rmem_max);
        224                space = min_t(u32, space, *window_clamp);
        225                while (space > 65535 && (*rcv_wscale) < 14) {
        226                        space >>= 1;
        227                        (*rcv_wscale)++;
        228                }
        229        }
        230

At this point the receiver of assume that window scaling is `0` in both directions of the communication.

**The Originator**

**References**

* http://lwn.net/Articles/92727/
* http://lxr.linux.no/
