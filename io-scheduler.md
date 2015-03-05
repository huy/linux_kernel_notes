# IO Scheduler

The responsibility of IO Scheduler is to schedule pending IO requests (read or write) "efficiently" with respect to 
physical characteristics of block devices (usually hard disk). The term schedule in this context means ordering and merging requests. IO Scheduler has to balance between latency with throughput.

Typical IO scheduler are

* elevator : no longer used, sort by block number
* deadline : replace elevator scheduler
* noop : simplest schduler using  queue
* Anticipatory : is replaced by CFQ
* CFQ - Completely Fair Queuing : default scheduler

IO Scheduler is specified in `[]` for each block device 

    # cat /sys/block/sda/queue/scheduler 
    noop [deadline]
    # echo noop > /sys/block/sda/queue/scheduler
    #cat /sys/block/sda/queue/scheduler 
    [noop] deadline

References

* http://www.linuxjournal.com/article/6931?page=0,1
