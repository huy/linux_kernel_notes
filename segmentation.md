## Segmentation


Segmentation is the part of memory management unit that translate logical address to linear address.


In Intel processor design
1. there is one Global Descriptor Table (GDT) of segment’s descriptors per CPU. The address and length of  GDT is stored in register gdtr.
2. each OS process can maintain it own Local Descriptor Table (LDT). The address and length of LDT is stored in register ldtr.


Each segment’s descriptor contains information about start address, size, type (whether it is user segment e.g. code, data or system segment e.g Task State Segment -TSS ) of the segment as well as access privilege  (Descriptor Privilege Level - DPL).


Logical memory address comprises of segment and offset within it. Register cs,ss,ds,gs,fs store segment identifier (also called segment selector), which has two parts
1. TI - Type Identification specify whether it is about segment descriptor  in GDT or LDT
2. pointer to the corresponding segment descriptor in GDT  or LDT




Linux made limited usage of segmentation. Linux setups one GDT per CPU. Inside GDT Linux maintains only one pair of descriptors for code and data/stack segment in user mode (__USER_CS, __USER_DS) and other pair in kernel mode (__KERNEL_CS,__KERNEL_DS) crossing all processes. All these segment’s descriptors have same start address (0x0), limit(0xFFFFF). The only different between kernel and it’s user counter part are privileges.


When OS switch from user mode to kernel mode, it load __KERNEL_CS into cs register, __KERNEL_DS into ds register. It loads __USER_CS into cs, __USER_DS into ds when doing reverse. __USER_DS/__KERNEL_DS descriptor is used for both data and stack segments.


Contents of all these descriptors are specified in cpu_gdt_table and do not change once they are initialized in memory by  cpu_init.


References
1. http://web.cs.wpi.edu/~cs3013/c07/lectures/Section09.1-Intel.pdf