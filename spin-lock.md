## Spin lock

When process is not given an access to the critical section,  with normal lock, process will be paused 
and moved to wait queue giving CPU to other process. 

Switching one process to other takes time because certain amount of machine’s instructions has to be performed 
to save running context from machine’s registers of one process to the memory and restore the context of other 
process from memory back to machine’s registers.

In multi processors machine, spin lock was invented to address this problem. With spin lock, the process try to 
enter a loop of nop (no operation - doing nothing) machine’s instruction for a while (the length is much shorter 
compare to context switch) then try to acquire  the lock again hoping that the process holding the lock runs on 
other processor will release it so there is no need to do expensive context switching.  


There is no meaning of doing spin lock on uni processor machine as if the process spins, other process that 
currently holds the lock can not get CPU to run in order to release the lock. 

Because normally the same code base is used to run in both uni and multi processors machine, the implementation 
of spin lock in case of uni processor will instead 

1. when running in user mode, in case of not able to acquire the lock: relinquish the processor and go to wait queue
2. when running in kernel mode, in case of successful acquisition of the lock, disable kernel preemption, 
which does not allow other process to get the CPU     

Spin lock is implemented and used in both user mode and kernel mode. The POSIX Thread API for spin lock is

1. pthread_spinlock_t spinlock
2. pthread_spin_lock(&spinlock)

References

1. http://www.moserware.com/2008/09/how-do-locks-lock.html#lockfn5
1. http://www.spinics.net/lists/newbies/msg40369.html
