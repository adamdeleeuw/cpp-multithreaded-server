#include "../include/Server.h"
using namespace std;

// NOTE: Still in rough shape, but making some progress.

/**
 * @result Sets up a new server.
 */
Server::Server() {
    Setup();
}

/**
 * @result Closes server and cleans up anything that must be cleaned.
 */
Server::~Server() {
    if (serverinfo != nullptr) freeaddrinfo(serverinfo);

    if (listenSocket_fd != SOCK_ERR) close(listenSocket_fd);
    if (clientSocket_fd != SOCK_ERR) close(clientSocket_fd);
}

/**
 * @result starts 
 */
void Server::Run() {
    prepareForAccept();
    Accept(); // call main loop to wait for and handle incoming connections
}


/* Private helper functions */

/** 
 * @result Gets necessary addrinfo, makes and binds a listening socket. 
 */
void Server::Setup() {
    setaddrinfo();

    status = getaddrinfo(NULL, PORT, &hints, &serverinfo); // get all required info
    if (status != 0) {
        throw runtime_error("getaddrinfo() errror: " + string(gai_strerror(status)));
    }

    Bind();   // binds this listening socket to the first result of addrinfo we can
    Listen(); // listen on the binded port
}

// Need to learn more about sigaction
void Server::prepareForAccept() {
    sa.sa_handler = sigchld_handler; // reap zombie processes (remove terminated child processes)
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        throw runtime_error("sigaction");
    }
}

/** 
 * @result Sets necessary addrinfo. 
 */
void Server::setaddrinfo() {
    memset(&hints, 0, sizeof(hints)); // clear struct to ensure its free
    hints.ai_family = AF_UNSPEC;      // IPv4 or IPv6, doesn't matter
    hints.ai_socktype = SOCK_STREAM;  // TCP stream socket
    hints.ai_flags = AI_PASSIVE;      // fill in localhost IP
    hints.ai_protocol = 0;            // choses the proper protocol for the given type of socket
}

void Server::Bind() {
    struct addrinfo* cr; // current result in the linked list of addrinfos

    // iterate through the result of getaddrinfo and bind to the first one we can
    for (cr = serverinfo; cr != nullptr; cr = cr->ai_next) {
        listenSocket_fd = socket(cr->ai_family, cr->ai_socktype, cr->ai_protocol);
        if (listenSocket_fd == SOCK_ERR) {
            perror("server: socket");
            continue; // curr result failed socket(), try next
        }

        if ((setsockopt(listenSocket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)))) {
            throw runtime_error("setsockopt() error");
        }

        if ((bind(listenSocket_fd, cr->ai_addr, cr->ai_addrlen)) == BIND_ERR) {
            perror("server: bind");
            continue; // curr result failed to bind, try next
        }

        break;
    } 

    freeaddrinfo(serverinfo); // no longer need this data
    serverinfo = nullptr;

    if (cr == nullptr) {
        throw runtime_error("server: bind failed for all results");
    }
}

void Server::Listen() {
    if ((listen(listenSocket_fd, BACKLOG)) == LISTEN_ERR) {
        throw runtime_error("sever: listen failed");
    }

    cout << "server: listening for connections..." << endl;
}

void Server::Accept() {
    cout << "server: waiting for connections..." << endl;
    
    for (;;) {
        sin_size = sizeof(client_adr);
        clientSocket_fd = accept(listenSocket_fd, (struct sockaddr*)&client_adr, &sin_size);

        if (clientSocket_fd == SOCK_ERR) {
            perror("server: accept");
            continue;
        }

        inet_ntop(client_adr.ss_family, get_in_addr((struct sockaddr*)&client_adr), s, sizeof(s));
        cout << "server: got connection from " << s << endl;

        // need to understand this better
        // note: will change to use thread() instead of fork() for modern C++
        if (!fork()) { // child process
            close(listenSocket_fd); // child does not need the listener
            listenSocket_fd = SOCK_ERR;

            if (send(clientSocket_fd, "Hello, world!", 13, 0) == SEND_ERR) {
                perror("server: send");
            }

            close(clientSocket_fd);
            clientSocket_fd = SOCK_ERR;

            exit(0); // child must exit
        }
        close(clientSocket_fd); // parent no longer needs child
        clientSocket_fd = SOCK_ERR;
    }
}

/* Non-member functions */

// Need to learn more about this
void sigchld_handler(int s) { // NOTE: non-memeber function
    (void)s;

    int saved_errno = errno; // save errno in case watipid() overwrites it
    while(waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
}

// Need to learn more about this
void* get_in_addr(struct sockaddr* sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
