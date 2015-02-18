## Signals

Signal is used to notify a process about external events (page fault, interrupt from keyboard) . Except real time signals, normal signals are not queued, kernel keep track of pending  signal for each process as bit mask. Also no other data is associated with signal.

Except KILL and STOP, signals can be blocked, default signal handlers can be ignored and overwritten.

Signals can be generated in program using kill, tkill, send_sig

To suspend a process run e.g.

     kill -SIGSTOP 3878

State of process can be observed by

     ps -eo pid,state,cmd
     
To resume the suspended process run

    kill -SIGCONT 3878
