## Slab allocator


Kernel allocate objects using slap layer, which provide a interface for create a cache for a specific type of object (e.g. task_struct), allocate an object from the cache and release object from cache.
Because memory are allocated in pages and size of objects are  usually smaller than the page size, this layer allows faster allocation of objects, eliminates memory waste and reduces memory fragmentation.
The implementation give kernel sufficient information for free memory by shrinking size of cache when needed.


References
1. http://linux.die.net/man/1/slabtop and cat /proc/slabinfo
2. http://www.puschitz.com/TuningLinuxForOracle.shtml
3. http://www.redhat.com/magazine/001nov04/features/vm/