# Network congestion management for incoming frames

There are two technique applied to network congestion management mainly to reduce CPU load.

1. reduce number of interrupt : this is done in interrupt handler of driver code by processing 
multi frame in one interrupt activation and polling

2. discard incoming frame as soon as possible: if the driver use NAPI which mean manage its own 
queue of incoming frames, it is up to driver to handle it. Otherwise kernel will handle it in `netif_rx` 
function by watching average of per CPU queue.
