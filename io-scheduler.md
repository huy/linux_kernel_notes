## IO Scheduler

The responsibility of IO Scheduler is to schedule pending IO requests (read or write) "efficiently" with respect to 
physical characteristics of block devices (usually hard disk). The term schedule in this context means ordering and merging requests. IO Scheduler has to balance between latency with throughput.

Typical IO scheduler are

* Elevator : no longer used, it sort requests by block number maintaining queue of sorted requests and serves requests as elevator maximizing throughput . When new requests with lower block number are constantly inserted in the head of the queue causes starvation of a request at the end of the queue.
* `deadline` : replace elevator scheduler
* `noop` : simplest schduler using  queue
* Anticipatory : is replaced by CFQ
* `cfq` - Completely Fair Queuing : default scheduler

IO Scheduler is specified in `[]` for each block device 

    /home/vagrant# cat /sys/block/sda/queue/scheduler
    noop [deadline] cfq
    /home/vagrant# echo cfq > /sys/block/sda/queue/scheduler
    /home/vagrant# cat /sys/block/sda/queue/scheduler
    noop deadline [cfq]


References

* http://www.linuxjournal.com/article/6931?page=0,1
