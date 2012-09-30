# From protocol handler to Socket Interface

The interface between user space program and kernel network is Socket. We access to socket using 

* socket api e.g.  `connect`, `accept`, `bind`, `listen`, `send`, `receive`  or
* file api e.g. `read`, `write`, `close`

Socket api is complete while file api some time offer only few operations, and alone is not enough 
to make socket work as the `open` is not provided, application end up with use only socket api.

**Socket data structures**

Socket

    struct socket {
        socket_state            state;
        unsigned long           flags;
        const struct proto_ops  *ops;
        struct fasync_struct    *fasync_list;
        struct file             *file;
        struct sock             *sk;
        wait_queue_head_t       wait;
        short                   type;
    };

Sock structure encapsulate internal implementation of socket

    struct sock {
            /*
             * Now struct inet_timewait_sock also uses sock_common, so please just
             * don't add nothing before this first member (__sk_common) --acme
             */
            struct sock_common      __sk_common;
    #define sk_family               __sk_common.skc_family
    #define sk_state                __sk_common.skc_state
    #define sk_reuse                __sk_common.skc_reuse
    #define sk_bound_dev_if         __sk_common.skc_bound_dev_if
    #define sk_node                 __sk_common.skc_node
    #define sk_bind_node            __sk_common.skc_bind_node
    #define sk_refcnt               __sk_common.skc_refcnt
    #define sk_hash                 __sk_common.skc_hash
    #define sk_prot                 __sk_common.skc_prot
            unsigned char           sk_shutdown : 2,
                                    sk_no_check : 2,
                                    sk_userlocks : 4;
            unsigned char           sk_protocol;
            unsigned short          sk_type;
            int                     sk_rcvbuf;
            socket_lock_t           sk_lock;
            wait_queue_head_t       *sk_sleep;
            struct dst_entry        *sk_dst_cache;
            struct xfrm_policy      *sk_policy[2];
            rwlock_t                sk_dst_lock;
            atomic_t                sk_rmem_alloc;
            atomic_t                sk_wmem_alloc;
            atomic_t                sk_omem_alloc;
            struct sk_buff_head     sk_receive_queue;
            struct sk_buff_head     sk_write_queue;
            struct sk_buff_head     sk_async_wait_queue;
            int                     sk_wmem_queued;
            int                     sk_forward_alloc;
            gfp_t                   sk_allocation;
            int                     sk_sndbuf;
            int                     sk_route_caps;
            int                     sk_gso_type;
            int                     sk_rcvlowat;
            unsigned long           sk_flags;
            unsigned long           sk_lingertime;
            /*
             * The backlog queue is special, it is always used with
             * the per-socket spinlock held and requires low latency
             * access. Therefore we special case it's implementation.
             */
            struct {
                    struct sk_buff *head;
                    struct sk_buff *tail;
            } sk_backlog;
            struct sk_buff_head     sk_error_queue;
            struct proto            *sk_prot_creator;
            rwlock_t                sk_callback_lock;
            int                     sk_err,
                                    sk_err_soft;
            unsigned short          sk_ack_backlog;
            unsigned short          sk_max_ack_backlog;
            __u32                   sk_priority;
            struct ucred            sk_peercred;
            long                    sk_rcvtimeo;
            long                    sk_sndtimeo;
            struct sk_filter        *sk_filter;
            void                    *sk_protinfo;
            struct timer_list       sk_timer;
            struct timeval          sk_stamp;
            struct socket           *sk_socket;
            void                    *sk_user_data;
            struct page             *sk_sndmsg_page;
            struct sk_buff          *sk_send_head;
            __u32                   sk_sndmsg_off;
            int                     sk_write_pending;
            void                    *sk_security;
            void                    (*sk_state_change)(struct sock *sk);
            void                    (*sk_data_ready)(struct sock *sk, int bytes);
            void                    (*sk_write_space)(struct sock *sk);
            void                    (*sk_error_report)(struct sock *sk);
            int                     (*sk_backlog_rcv)(struct sock *sk,
                                                      struct sk_buff *skb);
            void                    (*sk_create_child)(struct sock *sk, struct sock *newsk);
            void                    (*sk_destruct)(struct sock *sk);
    };
    

**Create a socket**
    
    asmlinkage long sys_socket(int family, int type, int protocol)
    {
            int retval;
            struct socket *sock;
    
    
            retval = sock_create(family, type, protocol, &sock);
            if (retval < 0)
                    goto out;
    
    
            retval = sock_map_fd(sock); /* assign function pointers for file interface*/
            if (retval < 0)
    ...
    }
    
    
    static int __sock_create(int family, int type, int protocol, struct socket **res, int kern)
    {
    …
            struct socket *sock;
    ...
            if (!(sock = sock_alloc())) {
    ...
            if ((err = net_families[family]->create(sock, protocol)) < 0) {
    ...
            *res = sock;
    …
    There is a function responsible for initializing socket for each socket family/domain
    
    
    struct net_proto_family {
            int             family;
            int             (*create)(struct socket *sock, int protocol);
    ...
    };
    
    
    int sock_register(struct net_proto_family *ops)
    {
    ...
            if (net_families[ops->family] == NULL) {
                    net_families[ops->family]=ops;
                    err = 0;
            }
    …

The socket family holding initialization function get registered at the system boot
    
    static int __init inet_init(void)
    {
    …
            (void)sock_register(&inet_family_ops);
    …
    
    
E.g. of  structure of socket family for IPv4 Internet protocols
    
    static struct net_proto_family inet_family_ops = {
            .family = PF_INET,
            .create = inet_create,
            .owner  = THIS_MODULE,
    };
    
    
    static int inet_create(struct socket *sock, int protocol)
    {
         struct sock *sk;
    
    
    ...
    lookup_protocol:
            err = -ESOCKTNOSUPPORT;
            rcu_read_lock();
            list_for_each_rcu(p, &inetsw[sock->type]) {
                    answer = list_entry(p, struct inet_protosw, list);
    
    
                    /* Check the non-wild match. */
                    if (protocol == answer->protocol) {
                            if (protocol != IPPROTO_IP)
                                    break;
    ...
    sock->ops = answer->ops; /* the socket operation is initialized from one registered in global variable inetsw */
    answer_prot = answer->prot; /* this will be used internally when ops actually delegate its work to a functions specified here*/
    ...
        sk = sk_alloc(PF_INET, GFP_KERNEL, answer_prot, 1);
    …
         sock_init_data(sock, sk)
    ….     
         if (sk->sk_prot->init) {
                    err = sk->sk_prot->init(sk);
    
    
The operation functions are specific to protocol (normally 0 as there is only one protocol) of the family (local, IPv4) 
and type of the socket (datagram,  stream or raw)  
    
    static int __init inet_init(void)
    {
    …
           for (q = inetsw_array; q < &inetsw_array[INETSW_ARRAY_LEN]; ++q)
                    inet_register_protosw(q);
    ...
       
    static struct inet_protosw inetsw_array[] =
    {
            {
                    .type =       SOCK_STREAM,
                    .protocol =   IPPROTO_TCP,
                    .prot =       &tcp_prot,
                    .ops =        &inet_stream_ops,
                    .capability = -1,
                    .no_check =   0,
                    .flags =      INET_PROTOSW_PERMANENT |
                                  INET_PROTOSW_ICSK,
            },
    
    
            {
                    .type =       SOCK_DGRAM,
                    .protocol =   IPPROTO_UDP,
                    .prot =       &udp_prot,
                    .ops =        &inet_dgram_ops, /*functions here will call those specified in .prot via internal socket*/
                    .capability = -1,
                    .no_check =   UDP_CSUM_DEFAULT,
                    .flags =      INET_PROTOSW_PERMANENT,
           },
    ...
    
    
    const struct proto_ops inet_dgram_ops = {
            .family            = PF_INET,
            .owner             = THIS_MODULE,
            .release           = inet_release,
            .bind              = inet_bind,
            .connect           = inet_dgram_connect,
    ...
            .recvmsg           = sock_common_recvmsg,
    ...
    
    
    void inet_register_protosw(struct inet_protosw *p)
    {
    ...
            answer = NULL;
            last_perm = &inetsw[p->type];
            list_for_each(lh, &inetsw[p->type]) {
                    answer = list_entry(lh, struct inet_protosw, list);
                    /* Check only the non-wild match. */
                    if (INET_PROTOSW_PERMANENT & answer->flags) {
                            if (protocol == answer->protocol)
                                    break;
                            last_perm = lh;
                    }
    …
          list_add_rcu(&p->list, last_perm);
    
    
    void sock_init_data(struct socket *sock, struct sock *sk)
    {
    ...
          if(sock)
            {
                    sk->sk_type     =       sock->type;
                    sk->sk_sleep    =       &sock->wait;
                    sock->sk        =       sk; /* wiring internal socket with socket*/
            } else
                    sk->sk_sleep    =       NULL;
    ..
            sk->sk_state_change     =       sock_def_wakeup;
            sk->sk_data_ready       =       sock_def_readable;
     
    
    struct sock *sk_alloc(int family, gfp_t priority,
                          struct proto *prot, int zero_it)
    {
            struct sock *sk = NULL;
    ...
                            sk->sk_prot = sk->sk_prot_creator = prot;
    
    struct proto udp_prot = {
            .name              = "UDP",
            .owner             = THIS_MODULE,
            .close             = udp_close,
            .connect           = ip4_datagram_connect,
            .disconnect        = udp_disconnect,
            .ioctl             = udp_ioctl,
            .destroy           = udp_destroy_sock,
            .setsockopt        = udp_setsockopt,
            .getsockopt        = udp_getsockopt,
            .sendmsg           = udp_sendmsg,
            .recvmsg           = udp_recvmsg,
            .sendpage          = udp_sendpage,
            .backlog_rcv       = __udp_queue_rcv_skb,
            .hash              = udp_v4_hash,
            .unhash            = udp_v4_unhash,
            .get_port          = udp_v4_get_port,
            .memory_allocated  = &udp_memory_allocated,
            .sysctl_mem        = sysctl_udp_mem,
            .sysctl_wmem       = &sysctl_udp_wmem_min,
            .sysctl_rmem       = &sysctl_udp_rmem_min,
            .obj_size          = sizeof(struct udp_sock),
    #ifdef CONFIG_COMPAT
            .compat_setsockopt = compat_udp_setsockopt,
            .compat_getsockopt = compat_udp_getsockopt,
    #endif
    };
    
Socket is attached to a file handle so we can operate with socket using file operation e.g. read, write

    static int sock_attach_fd(struct socket *sock, struct file *file)
    {
            struct qstr this;
            char name[32];
    
    
            this.len = sprintf(name, "[%lu]", SOCK_INODE(sock)->i_ino);
            this.name = name;
            this.hash = SOCK_INODE(sock)->i_ino;
    
    
            file->f_dentry = d_alloc(sock_mnt->mnt_sb->s_root, &this);
            if (unlikely(!file->f_dentry))
                    return -ENOMEM;
    
    
            file->f_dentry->d_op = &sockfs_dentry_operations;
            d_add(file->f_dentry, SOCK_INODE(sock));
            file->f_vfsmnt = mntget(sock_mnt);
            file->f_mapping = file->f_dentry->d_inode->i_mapping;
    
    
            sock->file = file;
            file->f_op = SOCK_INODE(sock)->i_fop = &socket_file_ops;
            file->f_mode = FMODE_READ | FMODE_WRITE;
            file->f_flags = O_RDWR;
            file->f_pos = 0;
            file->private_data = sock;
    
    
            return 0;
    }
    
    
**Receive data from socket**
    
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
