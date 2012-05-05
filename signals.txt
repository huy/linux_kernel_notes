## Signals


Signal is used to notify a process about external events (page fault, interrupt from keyboard) . Except real time signals, normal signals are not queued, kernel keep track of pending  signal for each process as bit mask. Also no other data is associated with signal.

Except KILL and STOP, signals can be blocked, default signal handlers can be ignored and overwritten.


Signals can be generated in program using kill, tkill, send_sig