## Process's scheduler

Process scheduler find the next eligible task and switch to the context of that task. It has to balance between responsiveness and efficency. The cost of context switch is not negligible. The option for tuning in case of `CSF` is `/proc/sys/kernel/sched_min_granularity_ns`  and `/proc/sys/kernel/sched_latency_ns`.

**Scheduluer and policy**

There are mutiple schedulers, each is responsible for a type of tasks. 

   stop_sched_class → rt_sched_class → fair_sched_class → idle_sched_class 

* `stop_sched_class` is for kernel tasks that load balances other tasks among CPU. 
* `rt_sched_class` is for real time tasks.
* `fair_sched_class` is for ordinary tasks. Most of tasks are of this type
* `ide_sched_class` is for lowest priority tasks that only run when there are no other runable tasks.

Linux provide syscall for assign a process to a specific scheduler policy `sched_setscheduler`. The following are policies applicable for non realtime scheduling

* `SCHED_OTHER`: the standard round-robin time-sharing policy;
* `SCHED_BATCH`: for "batch" style execution of processes; and
* `SCHED_IDLE`: for running very low priority background jobs

The realtime scheduling has following policies

* `SCHED_FIFO`: fifo scheduling
* `SCHED_RR`: round robind scheduling

**CFS**

Linux kernel from version 2.6.23 use Comppletly Faire Scheduler - CFS for process scheduling see [http://en.wikipedia.org/wiki/Completely_Fair_Scheduler].

Scheduler maintains per CPU run queue. CFS picks next task for particular CPU from its run queue.

**References**

* http://www.linuxjournal.com/magazine/completely-fair-scheduler
