## Understanding

TCP half close is term to describe situation that one end closes one direction of the communication channel (from it side), 
usually by sending FIN to other side. Upon receive ACK for that, it no longer sends any data but still can receive data from 
other.

The one sending `FIN` to other party changes it internal state to `FIN_WAIT_1`. Upon receiving the other end `ACK` it and 
changes to `CLOSE_WAIT`. After receiving `ACK` the sending one changes to `FIN_WAIT_2`.

An examplefor half close is one party sends text file as stream to other remote machine for sorting then get back the result.

    $rsh bsdi sort < datafile

After sending data, it 'half' closes the connection to indicate other side to start sort but keep reverse comunication direction
open to receive the result. 

Notes that this is not the same as half open, which is erroneous state, when one end has closed or aborted the connection without the knowledge of the other end. 
