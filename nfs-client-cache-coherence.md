
## NFS client cache

NFS client caches both data (content of a file) and metadata (file name and directory). When mounting with options to disable data and metadata cache, then every operations on NFS file system will send requests to NFS server and blocks until the server notifying a completion. So basically they behave as happen in the local file system.

Enable cache make thing faster so by default NFS file system is mounted with cache enabled. Unless we store database files on NFS or allow many NFS client read/write the same file concurrently, there is no harm to enable data cache. Why is that ? because e.g. when we read from file, data may be buffered by a library, so we don't know if the operation hit the OS kernel or thing just happens in the user space. So the idea of communication between 2 processes by read/write to the same file is rather awkward.

However meta data cache is different story, with meta data cache enabled if one client create a new file, close it and tell other the file name by some other mean. The other one may see that file or may not but if it see the file and read it then it will get a complete file. It is pretty good behavior.

NFS from v3 achieves it by flushing write data cache when closing file and invalidate any read data cache when medata has been changed (perhaps by looking at date modified).

**References**

* https://www.avidandrew.com/understanding-nfs-caching.html
* https://www.sebastien-han.fr/blog/2012/12/18/noac-performance-impact-on-web-applications/
* http://nfs.sourceforge.net/
