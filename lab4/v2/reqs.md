# General
- [] Makefile
- [] readme
- [] test tvpnc, tvpns, udppings udppingc

# Server
- [] Executable = tvpns
- [] tvpns \<ipv4\> \<port\> \<secret\>
- [] Seccret is 6 ASCII upper and lower case chars
- [] If binding fails (i.e. port is already in use) output suitable message and terminate
- [] STREAM socket

## Parent
- [] After accept returns, read() first two bytes and check that it contains the integer value 315.
    - [] If check fails, close sd and call accept again with og sd.
    - [] If the first byte *is* 315, check the next 6B to determine if it matches \<secret\>.
        - [] If secret does not match, close connection. And recall accept()
        - [] If match, check next 4bytes that it is final destination IPv4.
            - [] Next 2 bytes specify the final dest's port number
            - [] Last 4 bytes specify the source IPv4.
            - [] Make sure you are attending to big/little endian conversion.
- [] struct tunneltab forwardtab\[NUMSESSIONS\]:
    ```c
    struct tunneltab {
   unsigned long destaddr;
   unsigned short destpt;
   unsigned long sourceaddr;
   unsigned short sourcept;
   unsigned short tunnelpt; } 
   ```
- [] tvpns limits active forwarding sessions to NUMSESSIONS.
    - [] NUMSESSIONS is defined to be 8 in v2/tvpns.h
    - [] Initialise all fileds of forwardtab\[\] to 0
    - [] forwardtab\[i\].sourceaddr (i=0, 1, ..., NUMSESSIONS-1)
        * forwardtab\[i\] == 0 means that entry is free.
- [] The two fields sourcept and tunnelpt are populated by the child process that the parent process forks after updating forwardtab[]
- [] Before forking a child process, the parent stores the index of forwardtab[] for the new tunneling session in a local variable, unsigned int sessionindex, that the child process will use to carry out packet forwarding.

## Child
- [] Forked child creates UDP socket and binds it to a port starting at 55500.
    - [] If bind fails because of port being used, child increments the portNum until it succeeds.
- [] TCP socket by parent is used to communicate the 2B portNum to the client by calling write()
    - [] Preceded by the 6B secret key
    - [] Client confirms this secret key and then uses the information to communicate with the tunnelling server's IP via sendto()
    
- [] Second UDP socket for forwarding payload received from client
    - [] Binds to 57500 (incrementing the port number of binds() fails) which is stored in the field tunnelpt
- [] Responses from the destination will arrive at port tunnelpt. The child forwards to the client using the first UDP via sendto().
- [] Child monitors packets with SIGPOLL/SIGIO
    - [] Which calls recvfrom() to receive payload to recevfrom and sendto() to send to.
- [] Do the below
```
For the purpose of packet forwarding, a more suitable method is to block on the select() system call to monitor activity on multiple file descriptors. Since the child needs to monitor the TCP socket inherited from the parent for session termination, use select() to monitor activity on the three socket descriptors. If the child receives on the TCP socket a 6-byte message containing the secret key, it closes the TCP socket, frees up the session entry in forwardtab[], and calls exit(0) to terminate. 
```

// Bro, I don't understand this section

# Client
## tvpnc
- [] Sets up a tunnelling session by contacting tvpns.
    - `tvpnc \<tvpn_ipv4\> \<secret\> \<udpClient_ipv4\> \<finalDestUDP_ipv4\> \<finalDestUDP_Port\>`
- [] Prints out connection from child address

## UDP_Client
- `udppingc \<myaddr_ipv4\> \<secret\> \<portnum2\> \<pcount\>`
