## Access driver from user space

Driver is installed in kernel. In order for process in userspace to access it, we need to create special device file with major
number in file system. The driver module in turn register file system functions with the major number.

    result = register_chrdev(memory_major, "memory", &memory_fops);

Other way is to register using proc filesystem

    create_proc_read_entry("hello_world", 0, NULL, hello_read_proc, NULL) 

So when user process read, write to the special file, kernel invokes relevant registered file system functions. 
    
See example in 

* http://www.freesoftwaremagazine.com/articles/drivers_linux
* http://www.tldp.org/LDP/lkmpg/2.6/html/lkmpg.html
