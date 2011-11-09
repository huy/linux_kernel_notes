## Memory Management Unit


The MMU is hardware that translate logical memory address (used in machine instruction) to the physical one.   There are two step from logical address to linear address by segmentation unit and from linear address to physical address by paging unit.


OS Memory Management Subsystem manages memory by
1. setup and maintains a metadata about physical memory’ pages (global page descriptor table mem_map) so it will know if a memory’ page is in used or free, is modified i.e. dirty or clean.
1. set up/manipulate relevant virtual to physical memory translation tables (per process page directory and table)


Linux does not make use of segmentation so the translation table for segmentation is in fact common for all processes. However the translation table of paging unit is per process so changes each time at context switch.


32 bit linux kernel can use more than 4GB RAM (one process still has limitation of 4 GB, 1GB for kernel mode, 3GB for user mode) if  PAE (Physical Address Extension) is enabled in BIOS (grep -i PAE /proc/cpuinfo) and kernel (grep -i HIGHMEM /boot/config-’uname -r’)


Kernel maintain a meta information about memory pages. Due limitation of some hardware architectures (e.g. x86), pages are grouped in 3 memory zones DMA, NORMAL, HIGH.   To transfer data with devices kernel must use DMA zone.


Due to limitation of permanent mapping, unlike NORMAL zone, the memory in HIGH zone is not permanently mapped (put into page global directory) into kernel address space, i.e. is not possible to access directly but it requires first to allocate then map to kernel using kmap, which  returns address of the allocated page. After usage this should be un map using kunmap.  
The common pattern for using high memory is e.g. from reading file


int file_read_actor()
...
        kaddr = kmap(page);
        left = __copy_to_user(desc->arg.buf, kaddr + offset, size);
        kunmap(page);
...
or from reading socket
int skb_copy_datagram_iovec()
...
                       vaddr = kmap(page);
                        err = memcpy_toiovec(to, vaddr + frag->page_offset +
                                             offset - start, copy);
                        kunmap(page);
…


The 64 bit OS version does not have this issue, all memory pages are permanently mapped into NORMAL zone and kmap just return already mapped linear address while kunmap do nothing


void *kmap(struct page *page)
{
        might_sleep();
        if (!PageHighMem(page))
                return page_address(page);
        return kmap_high(page);
}


void kunmap(struct page *page)
{
        if (in_interrupt())
                BUG();
        if (!PageHighMem(page))
                return;
        kunmap_high(page);
}


References
1. http://www.spack.org/wiki/LinuxRamLimits
2. http://www.redhat.com/magazine/001nov04/features/vm/
3. http://blogs.oracle.com/gverma/2008/03/redhat_linux_kernels_and_proce_1.html
4. http://lists.us.dell.com/pipermail/linux-poweredge/2005-August/022327.html
5. http://en.wikipedia.org/wiki/Physical_Address_Extension