# Memory statistics

    cat /proc/meminfo

    MemTotal:       514916 kB
    MemFree:        142676 kB
    Buffers:         57068 kB
    Cached:         214008 kB          # Page cache
    SwapCached:          4 kB          # Cache of swap file
    Active:         217180 kB          #  Active/inactive pages used by PFCA
    Inactive:        99832 kB          #  
    HighTotal:           0 kB
    HighFree:            0 kB
    LowTotal:       514916 kB
    LowFree:        142676 kB
    SwapTotal:     1048568 kB
    SwapFree:      1048552 kB
    Dirty:               0 kB          # Cached file is modified and need to write to disk
    Writeback:           0 kB          #  Being written to disk
    AnonPages:       45932 kB
    Mapped:          13576 kB
    Slab:            47588 kB
    PageTables:       1676 kB
    NFS_Unstable:        0 kB
    Bounce:              0 kB
    CommitLimit:   1306024 kB
    Committed_AS:   170724 kB
    VmallocTotal:   507896 kB
    VmallocUsed:      5440 kB
    VmallocChunk:   502304 kB
    HugePages_Total:     0
    HugePages_Free:      0
    HugePages_Rsvd:      0
    Hugepagesize:     4096 kB
    

**Cache** 

physical memory used to cache files usually result from calling mmap

    fs/proc/proc_misc.c
    ...
    cached = global_page_state(NR_FILE_PAGES) -
                       total_swapcache_pages - i.bufferram;
    ...

    mm/filemap.c
    ...
    int add_to_page_cache(struct page *page, struct address_space *mapping,
               pgoff_t offset, gfp_t gfp_mask)
    {
    ...
                       __inc_zone_page_state(page, NR_FILE_PAGES);


**SwapCached**

When a page is swapped out, it goes through the same process as when writing a block of file to disk. 
So first go to cache and then the pdflush wil write it to block device. The benefit is possible merging 
many pages into single write (less IO operation).

**Active/Inactive**

Page Frame Reclaiming Algorithm maintains LRU lists of inactive and active page frames (physical memory page) 
that can be either assigned to processes or used as cache (excluding free pages).

    fs/proc/proc_misc.c
    ...
    get_zone_counts(&active, &inactive, &free);

**Dirty**

Amount of memory modified by processes and need to be written to files at some point of time. After calling sync, 
this should be zero

    fs/proc/proc_misc.c
    ...
               K(global_page_state(NR_FILE_DIRTY)),


**Writeback**

Amount of memory being written to files. Should be zero, if the disk is fast enough.

    fs/proc/proc_misc.c
    ...
              K(global_page_state(NR_WRITEBACK)),


    fs/buffer.c
    ...
    static int __block_write_full_page(struct inode *inode, struct page *page,
                       get_block_t *get_block, struct writeback_control *wbc)
    ...
       set_page_writeback(page);

**References**

1. http://docs.redhat.com/docs/en-US/Red_Hat_Enterprise_Linux/4/html/Reference_Guie/s2-proc-meminfo.html
2. fs/proc/proc_misc.c
3. mm/filemap.c
