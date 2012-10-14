## Paging unit

A memory page can be stored on disk or in a page frame (i.e. physical memory page) of physical memory. 
Paging unit of the processor works with fixed size page frame. Paging unit does translation linear address 
into physical address using Page table that must be initialize by the OS  before enabling Paging unit.

The 32 bit linear address consists of

1. Directory: The most significant 10 bits
2. Table: The intermediate 10 bits
3. Offset: The least significant 12 bits - imply that pagesize is 4 KB

Paging unit it is enabled by setting the PG flag of a control register named cr0. When PG = 0, linear 
addresses are interpreted as physical addresses.

The physical address of Page directory is stored in cr3 register. As each OS process see memory as contiguous 
region dedicated to it, the process needs its own Page directory, which is in mm field of task struct.

    crash> task | grep mm
     mm = 0xcd742e40,
     active_mm = 0xcd742e40,
    …
    struct mm_struct 0xcd742e40
    struct mm_struct {
     mmap = 0xce29d128,
     mm_rb = {
       rb_node = 0xcd64323c
     },
    ...
     free_area_cache = 3086761984,
     pgd = 0xcd743000,
    ...

The Page directory of a process map linear address in both user mode and kernel mode. It is divided into two parts

1. user part that maps logical address from 0x0-0xBFFFFFFF is different to each process
2. kernel part that maps logical address from 0xC0000000 to 0xFFFFFFFF is the same for all processes and equals 
master kernel Page Global Directory.

When a process is created the OS make sure that master kernel Page Global Directory is propagated in the Page 
directory of the new created process.
The code running in user mode can only access the first part while running in kernel mode can access both parts.

Because kernel processes does not need to access to the user part, it can use Page table of any processes so  
switch to kernel thread does not need to change Page directory table. This is a reason why task struct has 
both mm and active_mm pointers. An kernel thread always has mm=NULL while in user process mm equals active_mm.

    crash> task
    PID: 0      TASK: c068e3c0  CPU: 0   COMMAND: "swapper"
    struct task_struct {
     state = 0,
     thread_info = 0xc0708000,
     usage = {
       counter = 2
     },
    ...
    mm = 0x0,
    active_mm = 0x0,
    ...
    
Beside Address Translation Table, kernel must know the status of physical page frame whether a page is free, 
used by any process or has been modified. It keep these information in page descriptor table mem_map.  
Each page frame is represented by a 32 bytes descriptor, which mean around 1% of physical memory is waste 
(Linux use 4KB size page frame).

Linear memory accessible from each process can be viewed by looking at its mapping in proc file system. 
Each segment has start and end and permission. 

    [root@localhost kernel-internal-stuffs]# cat /proc/self/maps 
    008e8000-00903000 r-xp 00000000 fd:00 1737604    /lib/ld-2.5.so
    00903000-00904000 r-xp 0001a000 fd:00 1737604    /lib/ld-2.5.so
    00904000-00905000 rwxp 0001b000 fd:00 1737604    /lib/ld-2.5.so
    00907000-00a5a000 r-xp 00000000 fd:00 1737605    /lib/libc-2.5.so
    00a5a000-00a5c000 r-xp 00153000 fd:00 1737605    /lib/libc-2.5.so
    00a5c000-00a5d000 rwxp 00155000 fd:00 1737605    /lib/libc-2.5.so
    00a5d000-00a60000 rwxp 00a5d000 00:00 0 
    00a82000-00a83000 r-xp 00a82000 00:00 0          [vdso]
    08048000-0804d000 r-xp 00000000 fd:00 1678405    /bin/cat
    0804d000-0804e000 rw-p 00004000 fd:00 1678405    /bin/cat
    093b7000-093d8000 rw-p 093b7000 00:00 0          [heap]
    b7d3c000-b7f3c000 r--p 00000000 fd:00 714328     /usr/lib/locale/locale-archive
    b7f3c000-b7f3e000 rw-p b7f3c000 00:00 0 
    bf820000-bf835000 rw-p bffe9000 00:00 0          [stack]

or  using pmap pid

    [root@localhost kernel-internal-stuffs]# pmap -x 2982
    2982:   cscope
    Address   Kbytes     RSS   Dirty Mode   Mapping
    004af000       4       4       0 r-x--    [ anon ]
    008e8000     108      84       0 r-x--  ld-2.5.so
    00903000       4       4       4 r-x--  ld-2.5.so
    00904000       4       4       4 rwx--  ld-2.5.so
    00907000    1356     440       0 r-x--  libc-2.5.so
    00a5a000       8       8       4 r-x--  libc-2.5.so
    00a5c000       4       4       4 rwx--  libc-2.5.so
    00a5d000      12      12      12 rwx--    [ anon ]
    00a8d000      12       8       0 r-x--  libdl-2.5.so
    00a90000       4       4       0 r-x--  libdl-2.5.so
    00a91000       4       4       4 rwx--  libdl-2.5.so
    05d01000     256     152       0 r-x--  libncurses.so.5.5
    05d41000      32      12       4 rwx--  libncurses.so.5.5
    05d49000       4       4       4 rwx--    [ anon ]
    08048000     300      92       0 r-x--  cscope
    08093000       4       4       4 rw---  cscope
    08094000     104      44      44 rw---    [ anon ]
    09d55000    1208    1208    1208 rw---    [ anon ]
    b7fc7000       8       8       8 rw---    [ anon ]
    b7fd5000      12      12      12 rw---    [ anon ]
    bfc78000      84      32      32 rw---    [ stack ]
    -------- ------- ------- ------- -------
    total kB    3532       -       -       -

References

1. https://patchwork.kernel.org/patch/17020/
