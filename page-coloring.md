# Page coloring

Page coloring is a performance optimization designed to ensure that accesses to contiguous pages 
in virtual memory make the best use of the processor cache. 

Memory allocation unit of OS makes effort to avoid page aliasing (same page frame is placed more than one 
in cache because different virtual pages are mapped to the same page frame).

References

1. http://en.wikipedia.org/wiki/Cache_coloring
2. http://en.wikipedia.org/wiki/CPU_cache
3. http://www.freebsd.org/doc/en_US.ISO8859-1/articles/vm-design/page-coloring-optimizations.html
4. http://lwn.net/Articles/252125/
