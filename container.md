## Linux container

The concept container in linux refers to isolated light weight OS environment for running processes. A container appears as a complete OS (with root filesystem, network, hostname) dedicated to a process.

The linux kernel provide basic primitive enabling the creation of container, namely

* namespace: which prevent collision in using named resources e.g. process id, ipc, network port, filesystem, hostname
* cgroups: allocate physical resources (cpu, memory, io) to each container

References

* http://blog.dotcloud.com/under-the-hood-linux-kernels-on-dotcloud-part
* http://blog.dotcloud.com/kernel-secrets-from-the-paas-garage-part-24-c
