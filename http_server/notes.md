# Beej's guide to Network Programming

- Understanding Sockets
- Two types of sockets 
    - Datagram sockets (Unreliable, fast)
    - Stream sockets (Reliable, slow)
- Stream sockets are reliable because they use **TCP** protocol for data transmission.
- TCP or Transmission Control Protocol
    - Sender sends a stream of 8 bits (1 byte) 
    - Provides reliability via ACK and retransmission
    - Flow control by specifying window size during transmission
    - Allows multiple processes in single host to communicate via TCP using **Sockets**.
        - Socket is constituted of the triad
            : **Transport Protocol / IP Address / Port number**

# Links

- [Creating a socket](https://stackoverflow.com/questions/36069824/c-how-to-create-a-socket)
