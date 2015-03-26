## Process's scheduler

Process scheduler find the next eligible task and switch to the context of that task. It has to balance between responsiveness and efficency. The cost of context switch is not negligible. 

The option for tuning in case of `CFS` is `/proc/sys/kernel/sched_min_granularity_ns`  and `/proc/sys/kernel/sched_latency_ns`.

    # cat /proc/sys/kernel/sched_min_granularity_ns
    750000
    # cat /proc/sys/kernel/sched_latency_ns
    6000000

Scheduler maintains per CPU run queue and picks next task for particular CPU from its run queue. When a task calls `scheduler()` function, the scheduler code processes run queue associated with the CPU of the calling task.

**Scheduluer and policy**

There are mutiple schedulers, each is responsible for a type of tasks. 

    stop_sched_class → rt_sched_class → fair_sched_class → idle_sched_class 

* `stop_sched_class` is for kernel tasks that add/remove CPUs dynamically. 
 
* `rt_sched_class` is for real time tasks.
* `fair_sched_class` is for ordinary tasks. Most of tasks are of this type
* `ide_sched_class` is for lowest priority tasks that only run when there are no other runable tasks.

Linux provide syscall for assign a process to a specific scheduler policy `sched_setscheduler`. The following are policies applicable for non realtime scheduling

* `SCHED_OTHER`: the standard round-robin time-sharing policy, this is served by `fair_sched_class`
* `SCHED_BATCH`: for "batch" style execution of processes, this is served by `fair_sched_class`
* `SCHED_IDLE`: for running very low priority background jobs, this is served by `idle_sched_class`

The realtime scheduling has following policies

* `SCHED_FIFO`: fifo scheduling
* `SCHED_RR`: round robind scheduling

Scheduler class assigned to a process can be viewed using `class` field in `ps -o` command
 
    # ps -e -o pid,cmd,class 
    PID CMD                         CLS
    1 /sbin/init                  TS
    2 [kthreadd]                  TS
    ...
    6 [migration/0]               FF
    7 [watchdog/0]                FF

where 

    TS  SCHED_OTHER
    FF  SCHED_FIFO
    RR  SCHED_RR
    B   SCHED_BATCH
    ISO SCHED_ISO
    IDL SCHED_IDLE

We can change scheduler class using command `chrt`

    # ps -o pid,cmd,class,priority -p 1003
    PID CMD                         CLS PRI
    1003 cron                        TS   20
    # chrt -p -b 0 1003
    # ps -o pid,cmd,class,priority -p 1003
    PID CMD                         CLS PRI
    1003 cron                        B    20

**CFS**

Linux kernel from version 2.6.23 use Comppletly Faire Scheduler - CFS for process scheduling see [http://en.wikipedia.org/wiki/Completely_Fair_Scheduler].

`CFS` maintains per-task `vruntime`. As task is on CPU, this value get increased by time spending on CPU. The scheduler function check and pick task with minimum `vruntime` and assign CPU to it.

`prioprity` (i.e `nice`) is used as weight when adjusting `vruntime`.

**References**

* http://www.linuxjournal.com/magazine/completely-fair-scheduler
* https://www.kernel.org/doc/Documentation/scheduler/sched-design-CFS.txt
