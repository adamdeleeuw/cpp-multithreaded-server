#ifndef CLIENT
#define CLIENT

#define PORT         "3490" // the port client connects to
#define MAX_DATASIZE 100    // max number of bytes we can receive

#define SOCK_ERR  -1
#define CONCT_ERR -1
#define RECV_ERR  -1

#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

/* non-member function */
void* get_in_addr(struct sockaddr* sa);

class Client { 
    private:
        int socket_fd = -1; 
        int nbytes;
        struct addrinfo hints;
        struct addrinfo *serverinfo = NULL;
        char buf[MAX_DATASIZE + 1]; // +1 for null terminator
        char s[INET6_ADDRSTRLEN];
        const char* host = "127.0.0.1";
        int status;

        /* helper functions */
        void Setup();
        void setaddrinfo();
        void Connect();
        void Receive();

    public:
        Client();
        ~Client();
        void Run();
};

#endif