## Process execution

The processor can execute instruction of a process in either user mode (Current Privilege Level CPL=3 in 2 bits of cs) 
or kernel mode (CPL=0). The task struct holds information about process context in both user mode and kernel mode.

The process context switch can occur only in kernel mode. The contents of all registers used by a process in User Mode 
have already been saved on the Kernel Mode stack before performing process switching. This includes the contents of the 
ss and esp pair that specifies the User Mode stack pointer address

The set of data that must be loaded into the registers before the process resumes its execution on the CPU is called 
the hardware context . The hardware context is a subset of the process execution context, which includes all information 
needed for the process execution. In Linux, a part of the hardware context of a process is stored in the process 
descriptor, while the remaining part is saved in the Kernel Mode stack

When the hardware interrupt raises, processor save hardware context in current stack (can be either in user mode or 
kernel mode of any processes) and  processor jump to interrupt handler upon return it restore hardware context from current 
stack and continue its work.

Interrupt can be nested, which mean during an interrupt handler other interrupt can occur. Because of that an interrupt 
handler must never block, that is, no process switch can take place while an interrupt handler is running.

In fact, all the data needed to resume a nested kernel control path is stored in the Kernel Mode stack, which is 
tightly bound to the current process. So if the kernel switch to other process , Kernel mode stack will be changed 
and the code of the interrupt handler messes up.  

References

1. http://stackoverflow.com/questions/4732409/context-switch-in-interrupt-handlers
2. http://stackoverflow.com/questions/1053572/why-kernel-code-thread-executing-in-interrupt-context-cannot-sleep
