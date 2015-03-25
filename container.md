## Process container

The concept process container in linux refers to isolated light weight OS environment for running processes (also called OS level virtualization). A process container appears as a complete OS (with root filesystem, network, hostname) dedicated to a process. 

Comparing to virtual machine, process container (including lxc, docker, rocket) provides reasonable good isolated environment at much cheaper cost (in term time and resource).

The linux kernel provide features utilized by container's tools to create and manage of containers.

These are

* [namespace related syscalls](http://man7.org/linux/man-pages/man7/namespaces.7.html): which prevent collision in using named resources e.g. process id, ipc, network port, filesystem, hostname.
* [resource management cgroups](http://en.wikipedia.org/wiki/Cgroups): allocate physical resources (cpu, memory, io) to each container.

**Docker**

Docker is set of tools an infrastructure enabling creation and management of container. Docker is written in golang and uses libcontainer (the previous version used lxc) for accessing linux kernel namespace syscalls and cgroups.

**References**

* http://blog.dotcloud.com/under-the-hood-linux-kernels-on-dotcloud-part
* http://blog.dotcloud.com/kernel-secrets-from-the-paas-garage-part-24-c
* http://www.zdnet.com/article/docker-libcontainer-unifies-linux-container-powers/
