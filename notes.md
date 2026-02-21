My note below were gathered from Beej's Guide to Network Programming!!

Note in hindsight: I literally just copied down what Beej wrote in a lot of place or really poorly paraphrased, but I claim it helped me learn!

# What is a Socket? #
- Sockets enable communication to other programs using standard Unix file descriptors

Well, what is a *file descriptor*??
- "A file descriptor is simply an integer associated with an open file. But (and here’s the catch), that file can be a network connection, a FIFO, a pipe, a terminal, a real on-the-disk file, or just about anything else."

Beej predicted the natural question that follows, how does one obtain this file descriptor for network communication?
- Answer: "You make a call to the socket() system routine. It returns the socket descriptor, and you communicate through it using the specialized send() and recv() socket calls."

## Different Types of Sockets ##
There exists many type of sockets, but for the sake of this project I am only concerned with *internet sockets*.
Specifically, Beej mentions DARPA Internet addresses (internet sockets).

BUTTTT, there are two (many actually) types of internet socks:
1. **Stream Sockets (SOCK_STREAM):** Reliable two-way connected communication streams. If you output "1", "2", "3", they arrive in that exact order.
    - Uses **TCP** (Transmission Control Protocol) to ensure data arrives sequentially and error-free.
    - Examples: SSH, HTTP (Web Browsers), Telnet.

2. **Datagram Sockets (SOCK_DGRAM):** "Connectionless sockets." Unreliable because packets might arrive out of order or not at all. If they do arrive, the data is error-free.
    - Uses **UDP** (User Datagram Protocol).
    - No connection to maintain—just build a packet, slap an IP header on it, and send.
    - **Why use it?** SPEED. It’s "fire-and-forget."
    - Examples: Video streaming, games, VoIP (where a dropped frame is better than a huge lag spike).

# Network Theory
The key: **Data Encapsulation!**



- **The Flow:** Data is wrapped in a header (and sometimes footer) by one protocol, then that entire package is wrapped by the next layer (UDP -> IP -> Ethernet).
- **The Receiver:** The hardware strips the Ethernet header, the kernel strips IP/UDP, and the application finally gets the raw data.

## The Layered Network Model (The ISO's Open Systems Intercommunication (OSI) Model) ##
A universal language for networking to ensure different systems can talk to each other.



1. **Application:** Human-computer interaction (HTTP, FTP).
2. **Presentation:** Data representation and encryption.
3. **Session:** Managing sessions between applications.
4. **Transport:** End-to-end connections (TCP/UDP).
5. **Network:** Path determination and IP.
6. **Data Link:** Physical addressing (MAC).
7. **Physical:** Binary transmission (Cables/Wi-Fi).

**Beej’s Unix-Specific Model:**
- Application Layer
- Host-to-Host Transport Layer (TCP/UDP)
- Internet Layer (IP/Routing)
- Network Access Layer (Ethernet/Wi-Fi)

# IP Addresses #

1. **IPv4:** 32-bit (4 bytes), e.g., `192.0.2.111`. We’ve basically run out of these.
2. **IPv6:** 128-bit. The solution to IPv4 exhaustion.

## Subnets
IP addresses are split into a **network portion** and a **host portion**.
- **Netmask:** Used to "extract" the network ID using a bitwise-AND.
- **CIDR Notation:** The "New Style." e.g., `192.0.2.12/30` means the first 30 bits are the network.

## Port Numbers
If the IP address is the hotel street address, **ports are the room numbers.**
- 16-bit numbers used by TCP/UDP to direct data to specific services.
- **Common Ports:** HTTP (80), Telnet (23), SMTP (25), DOOM (666).
- **Privileged Ports:** Under 1024 (requires root).

# Byte Order (Endianness)
Your computer might store bytes in a different order than the network expects!

- **Big-Endian:** "Magnificent." Stores the most significant byte first. This is **Network Byte Order**.
- **Little-Endian:** "Lame." Stores the least significant byte first. This is what **Intel/x86 (Host Byte Order)** uses.

### The Conversion Functions
You must always convert data before sending it out or reading it in.

| Function | Description |
| :--- | :--- |
| **`htons()`** | host to network short (for ports) |
| **`htonl()`** | host to network long (for IPs) |
| **`ntohs()`** | network to host short |
| **`ntohl()`** | network to host long |

> **Pro Tip (supposedly):** Convert to Network Byte Order before data goes "out on the wire," and back to Host Byte Order when it comes "off the wire."

# Data Structures

**File descriptors** - type `int`, EASY!

Now it gets more complicated :/

## struct addrinfo
This is a more recent invention used to prep the socket address structures for later use. It’s also used in host name and service name lookups.

**THIS IS THE FIRST THING WE CALL WHEN MAKING A CONNECTION.**

```
struct addrinfo {
    int              ai_flags;     // AI_PASSIVE, AI_CANONNAME, etc.
    int              ai_family;    // AF_INET, AF_INET6, AF_UNSPEC
    int              ai_socktype;  // SOCK_STREAM, SOCK_DGRAM
    int              ai_protocol;  // use 0 for "any"
    size_t           ai_addrlen;   // size of ai_addr in bytes
    struct sockaddr *ai_addr;      // struct sockaddr_in or _in6
    char            *ai_canonname; // full canonical hostname

    struct addrinfo *ai_next;      // linked list, next node
};
```

- How to use: You load this struct up a bit, then call getaddrinfo(). It returns a pointer to a linked list of these structures filled with all the goodies you need.

- If you set ai_family to AF_UNSPEC, your code becomes IP-version agnostic (it doesn't care if it's IPv4 or IPv6).

- The ai_addr field points to a struct sockaddr, which is where the real IP/Port info lives.

### The so called "Nitty-Gritty"

1. **struct sockaddr** 
    This is the 'generic struct' that holds info for different types of sockets

    ```
    struct sockaddr {
    unsigned short    sa_family;    // address family, AF_xxx
    char              sa_data[14];  // 14 bytes of protocol address
    }; 
    ```


Lowkey tired of taking notes!

I think it's fine to refer to Beej's guide for technical implementation info, since its well organized and such. Will keep the rest of this file open for notes about things that I find very important (if I feel up for it). Even just trying to make this doc look nice felt like a waste of time. Oh well....

# Order of system calls (Beej's Ch5)

TO-DO: Add more info about params and purpose for each syscall, explain how it relates to the structs

For my HTTP server the order of syscalls is as follows:
```
getaddrinfo();
socket();
bind();
listen();
accept();
// send() and recv()
```

## 1. getaddrinfo()

```
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int getaddrinfo(const char *node,   // e.g. "www.example.com" or IP
                const char *service,  // e.g. "http" or port number
                const struct addrinfo *hints,
                struct addrinfo **res);
```

## 2. socket()

```
#include <sys/types.h>
#include <sys/socket.h>

int socket(int domain, int type, int protocol); 
```

## 3. bind()

```
#include <sys/types.h>
#include <sys/socket.h>

// lose the pesky "Address already in use" error message
int yes = 1;
setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);

// do bind
int bind(int sockfd, struct sockaddr *my_addr, int addrlen);
```

`struct sockaddr* my_addr` contains information about your address, namely, port and IP address

## 4. listen()

```
int listen(int sockfd, int backlog); 
```

`backlog` is the allowed number of connections on the incoming queue

## 5. accept()

```
#include <sys/types.h>
#include <sys/socket.h>

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen); 
```

`struct sockaddr *addr` a pointer to a local struct sockaddr_storage. This is where the information about the incoming connection will go (and with it you can determine which host is calling you from which port).

## 6. send() and recv()
 
**NOTE:** these are blocking calls

```
int send(int sockfd, const void *msg, int len, int flags); 

int recv(int sockfd, void *buf, int len, int flags);
```

## 7. close() and shutdown()

```
close(sockfd); 

int shutdown(int sockfd, int how); 
```

## 8. getpeername() and gethostname()

```
#include <sys/socket.h>

int getpeername(int sockfd, struct sockaddr *addr, int *addrlen);
```

```
#include <unistd.h>

/** Descr
  * @param...
  * @return
  */
int gethostname(char *hostname, size_t size); 
```

Important non memeber functions:

`void sigchld_handler(int s)` and `void* get_in_addr(struct sockaddr* sa)`


# Concurrency

## fork() vs. thread()
- fork(): creates a new child process (a copy of the parent process)
- thread(): creates a thread of execution within the SAME process

Analogy from Claude: a fork is like photocopying an entire book and giving it to somone to read, whereas a thread is like two people reading different chapters of the same book simultaneously

Therefore, thread is much more memory efficient because it requires less overhead (roughly goes from MBs to KBs) and execution is faster (roughly ms to mircoseconds)

Another big benefit is that since threads share memory, it's perfect for a server that must manage shared resources (cough cough will eb very helpful when I implement the thread pool)

In summary, for this project threads are better than forks because:
- they are more time and space efficient
- naturally work well for managing shared resources
- safer (RAII, smart ptrs, destructors)

The best resource I found for concurrent C++ programming is [here](https://www.bogotobogo.com/cplusplus/files/CplusplusConcurrencyInAction_PracticalMultithreading.pdf)

Wonderfully breaks down pretty much everything you need to know about conurrent C++, at least enough to do this project!

I'd recommend by starting with section 1.4, then continue reading into chap 2,3,4 and so on as you need.