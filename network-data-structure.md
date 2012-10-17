# Network data structure


The implementation of network use two fundamental data structure socket buffer `sk_buff` and  `net_device`.

`sk_buff` is where packets are stored and processed while net_device keep information about hardware device 
and pointer to driver’s functions put data into sk_buff as well as get data from it.

Other important is `softnet_data`, which is per CPU and contain information needed for communication between 
top half and bottom half

The `softnet_data` has the following noticeable fields

1. poll_list is the list of devices that are polled because they have a nonempty receive queue
2. output_queueis the list of devices that have something to transmit
