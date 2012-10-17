## Observe Kernel state  using crash

The most valuable tool for observing kernel state is crash, which can be used display kernel call trace, 
kernel memory structure, signal, irq, net socket, files

Example of examine kernel stack trace

    crash>ps
      PID    PPID  CPU   TASK    ST  %MEM     VSZ    RSS  COMM
         0      0   0  c068e3c0  RU   0.0       0      0  [swapper]
         1      0   0  dfc01aa0  IN   0.1    2160    688  init
         2      1   0  dfc01550  IN   0.0       0      0  [migration/0]
         3      1   0  dfc01000  IN   0.0       0      0  [ksoftirqd/0]
    
    
    crash> set 1
       PID: 1
    COMMAND: "init"
      TASK: dfc01aa0  [THREAD_INFO: dfc02000]
       CPU: 0
     STATE: TASK_INTERRUPTIBLE
    crash> bt
    PID: 1      TASK: dfc01aa0  CPU: 0   COMMAND: "init"
    #0 [dfc02af4] schedule at c061f412
    #1 [dfc02b6c] schedule_timeout at c061fb54
    #2 [dfc02b90] do_select at c0488ac3
    #3 [dfc02e34] core_sys_select at c0488dc6
    #4 [dfc02f74] sys_select at c048938d
    #5 [dfc02fb8] system_call at c0404f44
       EAX: 0000008e  EBX: 0000000b  ECX: bfc8e240  EDX: 00000000
       DS:  007b      ESI: 00000000  ES:  007b      EDI: bfc8e370
       SS:  007b      ESP: bfc8e20c  EBP: bfc8e508
       CS:  0073      EIP: 00909402  ERR: 0000008e  EFLAGS: 00000246
    
    
    crash>task
    PID: 1      TASK: dfc01aa0  CPU: 0   COMMAND: "init"
    struct task_struct {
     state = 1,
     thread_info = 0xdfc02000,
     usage = {
       counter = 2
     },
     flags = 4194560,
     lock_depth = -1,
     load_weight = 128,
     prio = 115,
     static_prio = 120,
     normal_prio = 115,
     run_list = {
       next = 0x100100,
       prev = 0x200200
     },
    
    
    crash> struct thread_info 0xdfc02000
    struct thread_info {
     task = 0xdfc01aa0,
     exec_domain = 0xc0693660,
     flags = 0,
     status = 0,
     cpu = 0,
     preempt_count = 0,
     addr_limit = {
       seg = 3221225472
     },
     sysenter_return = 0x909410,
  

References

1. http://people.redhat.com/anderson/crash_whitepaper/
2. http://codeascraft.etsy.com/2012/03/30/kernel-debugging-101/ - very practical useful using of crash
