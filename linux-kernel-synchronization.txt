## Synchronization


Semaphore is for synchronization between processes while mutex between threads of the same processes.


Memory barrier is mechanism to ensure that the machine executes instructions accessing memory(read/write) in the same order as the program has been written. Usually the processor/compiler change the order of the program execution due to the optimization.  This is important because when multi processes concurrently access memory the semantic depends on order of execution.  


Per CPU data is a technique, in which data is dedicated into one processor and thus no synchronization between CPUs is required and disable kernel preemption is sufficient.