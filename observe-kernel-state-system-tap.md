## Observe Kernel state - systemtap

stap - System Tap is tracing tool for both user and kernel.  System Tap's goal is to provide full system 
observability on production systems. On redhat System Tap requires kernel development, debug info and gcc compiler.

Example of tracing syscall open

    [root@localhost tap]#  cat open.stap
    probe syscall.open # whenever any process make syscall open then do at the beginning  
    {
     printf("%s(%d) open(%s)\n", execname(),pid(),argstr) # print name, pid and arguments passed to open
    }
    probe timer.ms(4000)  # watch only 4 seconds
    {
     exit()
    }
    
    [root@localhost tap]# stap open.stp
    pcscd(2487) open("/proc/bus/usb", O_RDONLY|O_DIRECTORY|O_LARGEFILE|O_NONBLOCK)
    pcscd(2487) open("/proc/bus/usb/002", O_RDONLY|O_DIRECTORY|O_LARGEFILE|O_NONBLOCK)
    pcscd(2487) open("/proc/bus/usb/002/001", O_RDWR)
    pcscd(2487) open("/proc/bus/usb/002/001", O_RDWR)


The different between stap and strace is the strace perform for one program while stap for all programs.

References

1. http://sourceware.org/systemtap/tutorial/Introduction.html
2. http://www.redbooks.ibm.com/redpapers/pdfs/redp4469.pdf
3. http://lwn.net/Articles/315022/

