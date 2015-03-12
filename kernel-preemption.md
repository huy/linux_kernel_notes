## User preemption

User preemption can occur

1. When returning to user-space from a system call
2. When returning to user-space from an interrupt handler

the kernel provides the `need_resched`  flag (one bit in thread_info.flags) to signify whether a reschedule 
should be performed. This flag is set by `scheduler_tick()` when a process should be preempted, and by 
`try_to_wake_up()` when a process that has a higher priority than the currently running process is awakened.

    static inline int need_resched(void)
    {
       return unlikely(test_thread_flag(TIF_NEED_RESCHED));
    }

Upon returning to user-space or returning from an interrupt, the `need_resched` flag is checked. If it is set, 
the kernel invokes the `schedule()` which may result in switching context to other task (i.e. other task runs on CPU).

The flag is per-process, and not simply global, because it is faster to access a value in the process 
descriptor (because of the speed of current and high probability of it being cache hot) than a global variable.

Historically, the flag was global before the 2.2 kernel. In 2.2 and 2.4, the flag was an int inside the 
task_struct. In 2.6, it was moved into a single bit of a special flag variable inside the thread_info structure.

    crash> struct task_struct {
     state = 0,
     thread_info = 0xc2546000,
    ...
    crash> struct thread_info 0xc2546000
    struct thread_info {
     task = 0xd1447550,
     exec_domain = 0xc0693660,
     flags = 128,
    ...

## Kernel Preemption


Non preemptive kernel does not switch a task when it is in kernel mode.  Task context switch only happens when the task
voluntarily call `schedule()` (i.e. cooperative kernel) or upon return from kernel mode to user mode (from system call or
interrupt handler)

Preemptive kernel however can preempt a task at kernel mode if it is safe to reschedule, which usually means the task 
holding no lock. The task `preempt_count` increases by 1 when a lock is acquired by  a task and decrements by 1 when a 
lock is released.

    crash> struct task_struct {
     state = 0,
     thread_info = 0xc2546000,
     ...
     
    crash> struct thread_info 0xc2546000
     struct thread_info {
     task = 0xd1447550,
     exec_domain = 0xc0693660,
     flags = 128,
     status = 0,
     cpu = 0,
     preempt_count = 0,
     ...


Kernel preemption can occur

1. When an interrupt handler exits, before returning to kernel-space. This is a case of an interrupt arises during a syscall
2. When kernel code becomes preemptible again, which means all the locks that the current task is holding are released, preempt_count returns to zero. The macro preempt_enable() which is called to check whether need_resched is set. If so, the `schedule()` is invoked.
3. If a task in the kernel explicitly calls `schedule()`
4. If a task in the kernel blocks which results in a call to `schedule()`
