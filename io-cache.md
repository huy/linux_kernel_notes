## IO cache


Read/write to block io (disk) go through page cache implemented using LRU/2. Two lists of pages are maintained active and inactive. Page Frame Reclaiming Algorithm (PFRA) manages cache eviction, which is only happened in inactive list. Pages are moved from inactive to active list when it is accessed.
Various parameters and the daemon pdflush are employed to ensure that cache will  be evicted to free memory when it is needed (dirty_background_ratio,dirty_ratio) as well as un-synchronized data are not remained too long in memory (dirty_expire_centisecs).


References
1. http://www.westnet.com/~gsmith/content/linux-pdflush.htm