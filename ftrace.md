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
 

**References**

* http://lwn.net/Articles/365835/
* http://lwn.net/Articles/366796/
