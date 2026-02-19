#ifndef SERVER
#define SERVER

#define PORT    "3490"
#define BACKLOG 10

#define SOCK_ERR   -1
#define BIND_ERR   -1
#define LISTEN_ERR -1
#define SEND_ERR   -1

#define NO_MORE_RECEIVES    0
#define NO_MORE_SENDS       1
#define NO_MORE_REC_OR_SEND 2

#include <iostream>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>

void sigchld_handler(int s);
void* get_in_addr(struct sockaddr* sa);

class Server {
    private:
        // init socket fd's and serverinfo in case error is thrown and destructor is call

        int listenSocket_fd = -1;           // holds the socket descriptor of a stream socket
        int clientSocket_fd = -1;           // connector's socket descriptor
        struct addrinfo hints;              // New addrinfo struct called 'hints' on stack
        struct addrinfo* serverinfo = NULL; // Pointer to the results from getaddrinfo() call
        struct sockaddr_storage client_adr; // connector's address info
        struct sigaction sa;
        socklen_t sin_size;
        char s[INET6_ADDRSTRLEN];
        int status;
        int yes = -1;

        /* Helper functions */
        void Setup();
        void prepareForAccept();
        void setaddrinfo();
        void Bind();
        void Listen();
        void Accept();
    public:
        Server();            // Constructor
        ~Server();           // Destructor
        void Run();          // Runs the server
};

#endif
