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

#define MSG "Hello World!\n"
#define MSG_SZ 13

#include "server_exceptions.h"
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
#include <thread>
#include <atomic>

using namespace std;

void* get_in_addr(struct sockaddr* sa);

enum class OpResult {
    SUCCESS,
    FAILURE,
    PARTIAL_SEND,
    CONNECTION_CLOSED
};

class Server {
    private:
        int listenSocket_fd;                // listener's socket descriptor
        struct addrinfo hints;              // New addrinfo struct called 'hints' on stack
        struct addrinfo* serverinfo;        // Pointer to the results from getaddrinfo() call
        atomic<bool> running;

        void Setup();
        void setaddrinfo();
        void Bind();
        void Listen();
        void Accept();
        OpResult handleClient(int csocket_fd);
    public:
        Server();
        ~Server();
        void Run();
};

#endif
