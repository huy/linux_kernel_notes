
## NFS client cache

NFS client caches both data (content of a file) and metadata (file name and directory). When mounting with options to disable data and metadata cache, then every operations on NFS file system will send requests to NFS server and blocks until the server notifying a completion. So basically they behave as happen in the local file system.

Enable cache make thing faster so by default NFS file system is mounted with cache enabled. Unless we store database files on NFS or allow many NFS client read/write the same file concurrently, there is no harm to enable data cache. Why is that ? because e.g. when we read from file, data may be buffered by a library, so we don't know if the operation hit the OS kernel or thing just happens in the user space. So the idea of communication between 2 processes by read/write to the same file is rather awkward.

However meta data cache is different story, with meta data cache enabled if one client create a new file, close it and tell other the file name by some other mean. The other one may see that file or may not but if it see the file and read it then it will get a complete file. It is pretty good behavior.
