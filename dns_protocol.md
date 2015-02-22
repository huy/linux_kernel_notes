## DNS protocol

**Recursive vs iterative**

When request sent to DNS server can be recursive or iterative. In the first case, if the server doesn't know the answer, 
it contact other servers to figure out and return the answer to the resolver (the client). On the other hand for iterative 
request the name server just return other name server for the resolver to contact them by itself.

