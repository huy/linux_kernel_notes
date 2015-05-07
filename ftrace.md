## FTrace 

Ftrace is kernel built in feature that enables tracing kernel function calls.

**Quick start**

change directory to `debugfs`
 
     # cd /sys/kernel/debug/tracing
     
check `current_tracer`

     # cat current_tracer
     nop
 
enable function trace
 
    # echo function > current_tracer
 
view trace

    # cat trace | less
    ...
            bash-8329  [000] d... 80604.787465: account_system_time <-__vtime_account_system
            bash-8329  [000] d... 80604.787466: cpuacct_account_field <-account_system_time

disable trace

    # echo nop > current_tracer

**References**

* http://lwn.net/Articles/365835/
* http://lwn.net/Articles/366796/
