## IO Scheduler

The responsibility of IO Scheduler is to schedule pending IO requests (read or write) against block devices (usually hard disk) "efficiently" with respect to physical characteristics of the devices. The term schedule in this context means ordering ,merging requests and decide which one is going to device first. IO Scheduler has to balance between latency with throughput.

Typical IO scheduler are

* Elevator : default in 2.4, no longer used.
* `deadline` : replaces Elevator, guarantees a start service time for a request.
* `noop` : simplest schduler using  queue.
* Anticipatory : idle after a read request to anticipate next closed read request. Is replaced by CFQ.
* `cfq` - Completely Fair Queuing : default scheduler.

IO Scheduler is specified in `[]` for each block device 

    /home/vagrant# cat /sys/block/sda/queue/scheduler
    noop [deadline] cfq
    /home/vagrant# echo cfq > /sys/block/sda/queue/scheduler
    /home/vagrant# cat /sys/block/sda/queue/scheduler
    noop deadline [cfq]

**Elevator**

It sort requests by sector's number maintaining queue of sorted requests and serves requests as elevator maximizing throughput. When new requests with lower sector's number are constantly inserted in the head of the queue causes starvation of a request at the end of the queue. Deadline I/O scheduler can outperform the CFQ I/O scheduler for certain multithreaded, database workloads.

**Deadline**

In addition to queue of requets sorted by block number, it maintains 2 deadline FIFO queue, one for read requests, other for write requests. The deadline for read requets queue is 500 ms and for write requests queue is 5 secs. The IO scheduler checks by looking at oldest requests if deadline ocurrs then it serves requests from deadline FIFO queue instead of sorted queue.

**noop**

Noop uses simple FIFO queue to merges and serves requests, no reordering based on sector number is performed. It assumes that OS has no productive way to optimize request order due to lack of information about physical devices. Example are SSD disk where seek time doesn't depend on sector number; Network Attached Storage, RAID and Tagged Command Queuing - TCQ, where the device manages request's queue by itself.

**Anticipatory**

Anticipatory scheduler tries to idle for short period (e.g. few ms) after a read operation in anticipation of another close-by read requests. It is remvoved because the same goal can be archived by tuning `slice_idle` of CFRQ.

**CFQ**

Maintains queue for synchronous requests per process. It allocates timeslice (based on IO priority of a process) for each queue to access disk. Asynchronous requests for all processes are batched together in fewer queues, one per priority. 

CFQ IO scheduler works in similar way as CPU scheduler. The detail is mentioned in [http://en.wikipedia.org/wiki/Completely_Fair_Scheduler], it prevent both starving and hogging. 

**References**

* http://en.wikipedia.org/wiki/Noop_scheduler
* http://en.wikipedia.org/wiki/Deadline_scheduler
* http://en.wikipedia.org/wiki/Anticipatory_scheduling
* http://en.wikipedia.org/wiki/CFQ
* http://man7.org/linux/man-pages/man2/ioprio_set.2.html
