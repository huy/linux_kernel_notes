
## NFS client cache

NFS client caches both data (content of a file) and metadata (file name and directory). When mounting with options to disable data and metadata cache, then every operations on NFS file system will send requests to NFS server and blocks until the server notifying a completion. So basically they behave as happen in the local file system.

**close-to-open cache consistency**

Enable cache make thing faster so by default NFS file system is mounted with cache enabled. Unless we store database files on NFS or allow many NFS client read/write the same file concurrently, we usually enable cache. The question is does NFS client implementation guarantee cache coherence in any form ?. It turn out that NFS client v3 introduced a concept of close to open consistency. It works in this scenario e.g.

A client create a new file, close it and tell other client the file name (by some other mean). The other one may see that file or may not but if it see the file and read it then it will get a complete file. It is pretty good behavior.

NFS v3 client achieves it by flushing data + meta data cache when closing file. The reader will access the meta data of the file then will invalidate any data cache when it seens that the medata has been changed (perhaps by looking at date modified). 

**References**

* https://www.avidandrew.com/understanding-nfs-caching.html
* https://www.sebastien-han.fr/blog/2012/12/18/noac-performance-impact-on-web-applications/
* https://serverfault.com/questions/611044/linux-read-disk-cache-and-nfs
* http://nfs.sourceforge.net/
