# Socket file api implementation

Because socket is kind of file, Linux implements special filesystem, inode for socket.

**Socket file system**

    [root@localhost ~]# cat /proc/filesystems 
    ..
    nodev        sockfs
    ...
    
    
    static int __init sock_init(void)
    {
    ...
            register_filesystem(&sock_fs_type);
            sock_mnt = kern_mount(&sock_fs_type);
    …
    }
    
    
    struct vfsmount *kern_mount(struct file_system_type *type)
    {
            return vfs_kern_mount(type, 0, type->name, NULL);
    }
    
    
    struct vfsmount *
    vfs_kern_mount(struct file_system_type *type, int flags, const char *name, void *data)
    {
    ..
    
    
            error = type->get_sb(type, flags, name, data, mnt);
    ..
    }
    
    static struct file_system_type sock_fs_type = {
            .name =         "sockfs",
            .get_sb =       sockfs_get_sb, /* return super block*/
            .kill_sb =      kill_anon_super,
    };
    
    static struct super_operations sockfs_ops = {
            .alloc_inode =  sock_alloc_inode,
            .destroy_inode =sock_destroy_inode,
            .statfs =       simple_statfs,
    };
    
    static int sockfs_get_sb(struct file_system_type *fs_type,
            int flags, const char *dev_name, void *data, struct vfsmount *mnt)
    {
            return get_sb_pseudo(fs_type, "socket:", &sockfs_ops, SOCKFS_MAGIC,
                                 mnt);
    }

**Socket inode**
    
    struct socket_alloc {
            struct socket socket;
            struct inode vfs_inode;
    };
    
    static struct inode *sock_alloc_inode(struct super_block *sb)
    {
            struct socket_alloc *ei;
            ei = (struct socket_alloc *)kmem_cache_alloc(sock_inode_cachep, SLAB_KERNEL);
            if (!ei)
                    return NULL;
            init_waitqueue_head(&ei->socket.wait);
    
    
            ei->socket.fasync_list = NULL;
            ei->socket.state = SS_UNCONNECTED;
            ei->socket.flags = 0;
            ei->socket.ops = NULL;
            ei->socket.sk = NULL;
            ei->socket.file = NULL;
            ei->socket.flags = 0;
    
    
            return &ei->vfs_inode;
    }
    
    
    static struct dentry_operations sockfs_dentry_operations = {
            .d_delete =     sockfs_delete_dentry,
    };
    
    
    struct file_operations socket_file_ops = {
            .owner =        THIS_MODULE,
            .llseek =       no_llseek,
            .aio_read =     sock_aio_read,
            .aio_write =    sock_aio_write,
            .poll =         sock_poll,
            .unlocked_ioctl = sock_ioctl,
    #ifdef CONFIG_COMPAT
            .compat_ioctl = compat_sock_ioctl,
    #endif
            .mmap =         sock_mmap,
            .open =         sock_no_open,   /* special open code to disallow open via /proc */
            .release =      sock_close,
            .fasync =       sock_fasync,
            .readv =        sock_readv,
            .writev =       sock_writev,
            .sendpage =     sock_sendpage,
            .splice_write = generic_splice_sendpage,
    };
    
**Socket file handler**

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
