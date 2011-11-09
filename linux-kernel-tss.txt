## TSS


Task State Segment -TSS identifier is stored in tr register. Because the processor uses different stack for user mode (privilege level 3) and protected mode (privilege level 0,1,2), stack switch occurs. The TSS is used to stored segment selector and offset of stack that will be used by a procedure being called when processor make a call from one privilege level to other more privilege level.         


In linux TSS is per CPU (not per process as originally intended by Intel design). TSS gets used when OS switch from user to kernel mode (Requester Privilege Level - RPL and DPL are different).   


When CPU switch from lower privilege (user mode) to higher privilege (kernel mode) , it fetch address (segment selector and pointer) of kernel stack from TSS. TSS also contains permission bitmap for I/O access (address used in in/out instruction) from user mode.


TSS segment is 236 bytes long, processes in User Mode are not allowed to access TSS segments. TSS occupies part of the kernel data segment. A TSS is different for each processor in the system. They are sequentially stored in the init_tss array, each element corresponds to one segment dedicated for one CPU.


References
1. http://en.wikipedia.org/wiki/Task_State_Segment