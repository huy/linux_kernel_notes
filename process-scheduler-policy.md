## Process's scheduler policy

Linux provide syscall for assign a process to a specific scheduler policy `sched_setscheduler`. The following are policies applicable for non realtime scheduling

* SCHED_OTHER   the standard round-robin time-sharing policy;
* SCHED_BATCH   for "batch" style execution of processes; and
* SCHED_IDLE    for running very low priority background jobs
