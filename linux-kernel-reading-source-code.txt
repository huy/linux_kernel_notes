## Reading source code


Kernel use extensively c extension and macro e.g.
typeof/__typeof__ ,  __attribute__


#define __get_cpu_var(var)        per_cpu__##var


static DEFINE_PER_CPU(struct socket *, __icmp_socket) = NULL;


/* Separate out the type, so (int[3], foo) works. */
#define DEFINE_PER_CPU(type, name) \
   __attribute__((__section__(".data.percpu"))) __typeof__(type) per_cpu__##name


crash> per_cpu____icmp_socket
PER-CPU DATA TYPE:
 struct socket *per_cpu____icmp_socket;
PER-CPU ADDRESSES:
 [0]: c1406ac0


crash> struct socket per_cpu____icmp_socket
struct socket {
 state = 3518432990,
 flags = 2251442003,
 ops = 0xe9518e15,
 fasync_list = 0x671341b9,
 file = 0x41b13d1,
 sk = 0xf3e74312,
 wait = {
   lock = {
     raw_lock = {
       slock = 1482566028
     }
   },
   task_list = {
     next = 0xa9ea9e0f,
     prev = 0x5b6959e4
   }
 },
 type = -24496
}


__attribute__(__section__) is used to place global/static memory variable into non-default segment so the linker later will map them to appropriate desired memory location. An example is a variable that maps to DMA location of device so read/write to that variable mean get/send data to the device.


unsigned long long denotes 64 bit integer in 32 bit architecture


fastcall indicates that compiler will try to use registers for passing parameters to the function instead of stack as normal.


Some code of kernel is platform dependent so are written in assembly language


References
1. http://gcc.gnu.org/onlinedocs/gcc/C-Extensions.html#C-Extensions
2. http://en.wikipedia.org/wiki/X86_assembly_language
3. http://www.ibm.com/developerworks/library/l-gas-nasm.html
4. http://lxr.linux.no/