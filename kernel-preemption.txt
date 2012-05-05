## Kernel Preemption


Non preemptive kernel will not switch a task when it is in kernel mode.  Task context switch only happens when the task voluntarily call schedule (i.e. cooperative kernel) or upon return from kernel mode to user mode (from system call or interrupt handler).


Preemptive kernel however can preempt a task at kernel mode if it is safe to reschedule, which usually means the task holding no lock. The task preempt_count increases by 1 when a lock is acquired by  a task and decrements by 1 when a lock is released.


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
2. When kernel code becomes preemptible again, which means all the locks that the current task is holding are released, preempt_count returns to zero. The macro preempt_enable() which is called to check whether need_resched is set. If so, the schedule() is invoked.
3. If a task in the kernel explicitly calls schedule()
4. If a task in the kernel blocks (which results in a call to schedule() )