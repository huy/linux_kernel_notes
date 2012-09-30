## Kernel panic, oops, call trace


Unlike kernel panic, which can not be recovered and reboot the system, when the kernel detects a oops’s problem, it
prints kernel state including call trace and kills any offending process.


E.g of call trace


    [<c01511a7>] __alloc_pages+0x29f/0x2b1
    [<c014cfc3>] find_or_create_page+0x39/0x72
    [<c01717d8>] grow_dev_page+0x2a/0x1eb
    [<c0171ac7>] __getblk_slow+0x12e/0x159
    [<c0171e49>] __getblk+0x3f/0x49


Description of each field

1. address of function in System.map, kernel use this address to figure out the function name
2. function name
3. the offset from start of the function in bytes, we use this to figure out which line was currently running
4. the size of the function in bytes

To find the line of function, we need to install crash

    crash> bt
    PID: 2489   TASK: d3f86aa0  CPU: 0   COMMAND: "top"
    #0 [cd264af4] schedule at c061f412
    #1 [cd264b6c] schedule_timeout at c061fb54
    #2 [cd264b90] do_select at c0488ac3
    #3 [cd264e34] core_sys_select at c0488dc6
    #4 [cd264f74] sys_select at c048938d
    #5 [cd264fb8] system_call at c0404f44
       EAX: ffffffda  EBX: 00000001  ECX: bfde8ddc  EDX: 00000000
       DS:  007b      ESI: 00000000  ES:  007b      EDI: 08056a00
       SS:  007b      ESP: bfde8b3c  EBP: bfde9488
       CS:  0073      EIP: 002ee402  ERR: 0000008e  EFLAGS: 00000246
    crash> sym c048938d
    c048938d (T) sys_select+149  ../debug/kernel-2.6.18/linux-2.6.18.i686/fs/select.c: 407
    crash>

Description of each field

1. address of stack at the time of invoking next upper function, by looking at this we know the parameters passing to
2. name of kernel function
3. address of an instruction of the function

system_call & sys_select means the task make call to select the user stack trace would be

    sh-3.2# pstack 2489
    #0  0x00d51402 in __kernel_vsyscall ()
    #1  0x007d9c3d in ___newselect_nocancel () from /lib/libc.so.6
    #2  0x08051372 in signal_name_to_number ()
    #3  0x00724e9c in __libc_start_main () from /lib/libc.so.6
    #4  0x08049741 in signal_name_to_number ()


References

1. http://en.wikipedia.org/wiki/System.map
2. http://madwifi-project.org/wiki/DevDocs/KernelOops
3. http://magazine.redhat.com/2007/08/15/a-quick-overview-of-linux-kernel-crash-dump-analysis/
4. https://access.redhat.com/kb/docs/DOC-2024