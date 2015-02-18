## How debugger works

The implementation of debugger uses of system call `ptrace`.

The debugger in this case a tracer establishes parent/child relationship (in this case real_parent and parent are different) 
with a process being traced then call `ptrace` with pid of the tracee. The tracer then can receive (by calling system call `wait`/`waitpid`) any signals (except KILL) sent to the tracee.

The debugger ask the kernel to step (`PTRACE_SINGLESTEP`), continue execution (`PTRACE_CONT`) the process being traced.

**breakpoint**

The debugger can set a breakpoint by changing an instruction (`PTRACE_POKETEXT`) at desire location with INT 3 (raise `SIGTRAP`).
Then it can examine, modify content of memory and registries using `PTRACE_PEEKTEXT`, `PTRACE_PEEKDATA`, `PTRACE_GETREGS`.
Finally rewinds instruction pointer (`regs.eip=-1+`, `PTRACE_SETREGS`), restores original instruction then continue 
execution (`PTRACE_CONT`) .

**References**

1. http://eli.thegreenplace.net/2011/01/23/how-debuggers-work-part-1/
2. http://www.linuxjournal.com/article/6100?page=0,2
