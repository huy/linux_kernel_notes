## IO Scheduler

The responsibility of IO Scheduler is to schedule pending IO requests (read or write) "efficiently" with respect to 
physical characteristics of block devices (usually hard disk). The term schedule in this context means ordering and merging requests. IO Scheduler has to balance between latency with throughput.

Typical IO scheduler are

* Elevator : default in 2.4, no longer used.
* `deadline` : replaces Elevator.
* `noop` : simplest schduler using  queue
* Anticipatory : is replaced by CFQ
* `cfq` - Completely Fair Queuing : default scheduler

**Elevator**

It sort requests by block number maintaining queue of sorted requests and serves requests as elevator maximizing throughput . When new requests with lower block number are constantly inserted in the head of the queue causes starvation of a request at the end of the queue

**Deadline**

In addition to queue of requets sorted by block number, it maintains 2 deadline FIFO queue, one for read requests, other for write requests. The deadline for read requets queue is 500 ms and for write requests queue is 5 secs. The IO scheduler checks by looking at oldest requests if deadline ocurrs then it serve requests from deadline FIFO queue instead of sorted queue.

IO Scheduler is specified in `[]` for each block device 

    /home/vagrant# cat /sys/block/sda/queue/scheduler
    noop [deadline] cfq
    /home/vagrant# echo cfq > /sys/block/sda/queue/scheduler
    /home/vagrant# cat /sys/block/sda/queue/scheduler
    noop deadline [cfq]


References

* http://www.linuxjournal.com/article/6931?page=0,1
