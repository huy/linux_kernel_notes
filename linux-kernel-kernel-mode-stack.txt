# Kernel mode stack


Every process (LWP) has its own stack when running in kernel mode. The size of kernel mode stack is usually 4 or 8 KB (configured at kernel’s compile time). However thread_info (usually 56 byte) is allocated at the bottom of the stack. Note that stack grows from top to bottom, which mean pushing data to stack decreasing address of stack pointer, while popping data from stack increasing its address.


One way to get kernel mode stack size is used crash


crash> print sizeof(struct thread_info)
$5 = 56
crash> print sizeof(union thread_union)
$6 = 4096
crash>


thread_info is stored in kernel mode stack is for reason of efficiency. There are only few assembly instructions required to get address of process which is first field in thread_info struct


References
1. http://notoveryet.wordpress.com/2009/07/09/linux-kernel-bits-which-i-feel-excited-about/