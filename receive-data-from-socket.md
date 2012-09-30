# Receive data from socket
    
    asmlinkage long sys_recvfrom(int fd, void __user * ubuf, size_t size, unsigned flags,
                                 struct sockaddr __user *addr, int __user *addr_len)
    {
    …
            sock_file = fget_light(fd, &fput_needed);
            if (!sock_file)
                    return -EBADF;
    ...
            sock = sock_from_file(sock_file, &err);
            if (!sock)
                    goto out;
    …
            err=sock_recvmsg(sock, &msg, size, flags);
    …
    
    
The syscall first look for a socket from file descriptor and call relevent function using the socket data structure

    int sock_recvmsg(struct socket *sock, struct msghdr *msg,
                     size_t size, int flags)
    {
    ...
            ret = __sock_recvmsg(&iocb, sock, msg, size, flags);
    ...
    }
    
    
    static inline int __sock_recvmsg(struct kiocb *iocb, struct socket *sock,
                                     struct msghdr *msg, size_t size, int flags)
    {
    ...
           err = sock->ops->recvmsg(iocb, sock, msg, size, flags); 
    …
    
    
ops is protocol operation struct holding pointers to functions operating on socket file descriptor. 
When creating socket this field is filled depending mostly on domain/family and type
    
    int sock_common_recvmsg(struct kiocb *iocb, struct socket *sock,
                            struct msghdr *msg, size_t size, int flags)
    {
            struct sock *sk = sock->sk;
            int addr_len = 0;
            int err;
           /* this will delegate its work to internal sock*/
            err = sk->sk_prot->recvmsg(iocb, sk, msg, size, flags & MSG_DONTWAIT,
                                       flags & ~MSG_DONTWAIT, &addr_len);
            if (err >= 0)
                    msg->msg_namelen = addr_len;
            return err;
    }
    
    static int udp_recvmsg(struct kiocb *iocb, struct sock *sk, struct msghdr *msg,
                           size_t len, int noblock, int flags, int *addr_len)
    {
            struct inet_sock *inet = inet_sk(sk);
            struct sockaddr_in *sin = (struct sockaddr_in *)msg->msg_name;
            struct sk_buff *skb;
            int copied, err;
            int peeked;
    
    
            /*
             *      Check any passed addresses
             */
            if (addr_len)
                    *addr_len=sizeof(*sin);
    
    
            if (flags & MSG_ERRQUEUE)
                    return ip_recv_error(sk, msg, len);
    
    
    try_again:
            skb = __skb_recv_datagram(sk, flags | (noblock ? MSG_DONTWAIT : 0),
                                      &peeked, &err);
    ...
    
    
    struct sk_buff *__skb_recv_datagram(struct sock *sk, unsigned flags,
                                      int *peeked, int *err)
    {
    ...
    
    
            timeo = sock_rcvtimeo(sk, flags & MSG_DONTWAIT);
    …
          do {
                    /* Again only user level code calls this function, so nothing
                     * interrupt level will suddenly eat the receive_queue.
                     *
                     * Look at current nfs client by the way...
                     * However, this function was corrent in any case. 8)
                     */
    
    
                    unsigned long cpu_flags;
    
    
                    if (flags & MSG_PEEK) {
                            spin_lock_irqsave(&sk->sk_receive_queue.lock,
                                              cpu_flags);
                            skb = skb_peek(&sk->sk_receive_queue);
                            if (skb) {
                                    *peeked = skb->peeked;
                                    skb->peeked = 1;
                                    atomic_inc(&skb->users);
                            }
                            spin_unlock_irqrestore(&sk->sk_receive_queue.lock,
                                                   cpu_flags);
                    } else {
                            skb = skb_dequeue(&sk->sk_receive_queue);
                            if (skb)
                                    *peeked = skb->peeked;
                    }
                    if (skb)
                            return skb;
    
    
                    /* User doesn't want to wait */
                    error = -EAGAIN;
                    if (!timeo)
                            goto no_packet;
    
    
            } while (!wait_for_packet(sk, err, &timeo));
    …
    
    
    static int wait_for_packet(struct sock *sk, int *err, long *timeo_p)
    {
            int error;
            DEFINE_WAIT(wait);
    
    
            prepare_to_wait_exclusive(sk->sk_sleep, &wait, TASK_INTERRUPTIBLE);
    ...
           *timeo_p = schedule_timeout(*timeo_p);
    


References

1. http://www.ibm.com/developerworks/linux/library/l-hisock/index.html
2. http://www.haifux.org/lectures/217/netLec5.pdf
