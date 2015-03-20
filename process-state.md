## Process state

Process state can be observed by `ps` command

    root@gerrit01:/home/vagrant# ps auxf
    USER       PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
    root         2  0.0  0.0      0     0 ?        S    Mar19   0:00 [kthreadd]
    root         3  0.0  0.0      0     0 ?        S    Mar19   0:01  \_ [ksoftirqd/0]
    root         5  0.0  0.0      0     0 ?        S<   Mar19   0:00  \_ [kworker/0:0H]

Process state can be one of the following

    PROCESS STATE CODES
               D    uninterruptible sleep (usually IO)
               R    running or runnable (on run queue)
               S    interruptible sleep (waiting for an event to complete)
               T    stopped, either by a job control signal or because it is being traced
               W    paging (not valid since the 2.6.xx kernel)
               X    dead (should never be seen)
               Z    defunct ("zombie") process, terminated but not reaped by its parent
    For BSD formats and when the stat keyword is used, additional characters may be displayed:
               <    high-priority (not nice to other users)
               N    low-priority (nice to other users)
               L    has pages locked into memory (for real-time and custom IO)
               s    is a session leader
               l    is multi-threaded (using CLONE_THREAD, like NPTL pthreads do)
               +    is in the foreground process group

The most interesting is uninterruptible sleep `D`, which can be watched using

      $while true; do date; ps auxf | awk '{if($8=="D") print $0;}'; sleep 1; done
      
Process in in uninterruptible sleep, means that it runs in kernel mode and can't be preempted. The CPU is not used but can't 
be reused to run other tasks because kernel either is holding lock or its data structure is not protected. It is a sign of
deficiency of certain parts of the kernel and should be avoided as much as possible.


