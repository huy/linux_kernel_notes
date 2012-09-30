# Example of troubleshooting JVM

Identify JVM process and thread (LWP), that causes a problem e.g. high CPU usage

    ps -eLo pid,lwp,pcpu,vsz,comm

Run gdb attach to process and display stacktrace of problematic thread, remember lwp

    gdb -p <jvm_process_id>
    gdb> info threads
                                # identify thread number, its lwp matchs the problematic lwp in ps command
    gdb> thread <thread_number> # switch to that thread
    gdb> bt                     #  get stacktrace
    …