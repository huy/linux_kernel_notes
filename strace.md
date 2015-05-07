## strace quick start

**Identify running process**

    # ps -ef | grep Gerrit
    gerrit   17549     1  0 May06 pts/1    00:02:45 GerritCodeReview -server
    
    
**Run strace**

    strace -f -p 17549 -s 128 -o /tmp/strace.lo

* the `-f` means trace all childrens/threads of the given process.
* the `-s 128` means buffer for store syscall's parameter is 128 byte. The default is 80, which is usually too small
* the `-o` redirect tracing info into a file for filtering later as amount of tracing info is usually overwhelming

With the knowledge if syscall we can ask strace to trace only specified syscalls or group of syscalls

    strace -f -p 17549 -s 128 -e trace=lstat,open
    
strace predefine a few groups of syscalls such as `file`, `process`, `network`, `rpc`

    strace -f -p 17549 -s 128 -e trace=file
