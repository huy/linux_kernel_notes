## DNS protocol

**Recursive vs iterative**

When request sent to DNS server can be recursive or iterative. In the first case, if the server doesn't know the answer, 
it contact other servers to figure out and return the answer to the resolver (the client). On the other hand for iterative 
request the name server just return other name server for the resolver to contact them by itself.

By default resolver use recursive query, it can be show using `tshark -V`
    
    Domain Name System (query)
        Transaction ID: 0x0965
        Flags: 0x0100 Standard query
            0... .... .... .... = Response: Message is a query
            .000 0... .... .... = Opcode: Standard query (0)
            .... ..0. .... .... = Truncated: Message is not truncated
            .... ...1 .... .... = Recursion desired: Do query recursively
            .... .... .0.. .... = Z: reserved (0)
            .... .... ...0 .... = Non-authenticated data: Unacceptable
        Questions: 1


