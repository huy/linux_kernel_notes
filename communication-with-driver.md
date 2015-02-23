## Access driver from user space

Driver is installed in kernel. In order for process in userspace to access it, we need to create special device file with major
number in file system. The driver module in turn register file system functions with the major number.

So when user process read, write to the special file, kernel invokes relevant registered file system functions. See example in http://www.freesoftwaremagazine.com/articles/drivers_linux

