#include "../include/Server.h"
using namespace std;

/**
 * Default constructor: constructs a new TCP server instance.
 * @result Sets up a new server
 */
Server::Server() : serverinfo(nullptr), listenSocket_fd(SOCK_ERR), clientSocket_fd(SOCK_ERR) {
    Setup();
}

/**
 * Desctructor
 * @result Closes listening socket and cleans up anything leftover data.
 */
Server::~Server() {
    if (serverinfo != nullptr) freeaddrinfo(serverinfo);
    if (listenSocket_fd != SOCK_ERR) close(listenSocket_fd);
    // client threads close their own sockets
}

/**
 * Server "play button".
 * @result Starts server (allows connections to be made)
 */
void Server::Run() {
    Accept(); // call main loop to wait for and handle incoming connections
}


/* Private helper functions */

/** 
 * Sets data, creates a listening stream socket, binds it, and listens on the binded port.
 * @result The server is prepared to run.
 * @throws If getaddrinfo() fails
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

/**
 * Initializes address info hints structure with default TCP settings.
 * @result Fills hints struct for IPvX (4/6 - either or) TCP stream socket
 */
void Server::setaddrinfo() {
    memset(&hints, 0, sizeof(hints)); // clear struct to ensure its free
    hints.ai_family = AF_UNSPEC;      // IPv4 or IPv6, doesn't matter
    hints.ai_socktype = SOCK_STREAM;  // TCP stream socket
    hints.ai_flags = AI_PASSIVE;      // fill in localhost IP
    hints.ai_protocol = 0;            // choses the proper protocol for the given type of socket
}

/**
 * Creates a socket and binds it to a port.
 * @throws If bind fails.
 */
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

/**
 * Listens for incoming client connections.
 * @throws If listening fails
 */
void Server::Listen() {
    if ((listen(listenSocket_fd, BACKLOG)) == LISTEN_ERR) {
        throw runtime_error("sever: listen failed");
    }

    cout << "server: listening for connections..." << endl;
}

/**
 * Performs the main "wait and accept when possible" loop.
 * @result Waits for incoming client connections. If one is found, it is accepted then handled.
 */
void Server::Accept() {
    cout << "server: waiting for connections..." << endl;
    
    for (;;) {
        sin_size = sizeof(client_adr);
        clientSocket_fd = accept(listenSocket_fd, (struct sockaddr*)&client_adr, &sin_size);

        if (clientSocket_fd == SOCK_ERR) {
            perror("server: accept");
            continue;
        }

        // converts network address structure to a string of characters
        inet_ntop(client_adr.ss_family, get_in_addr((struct sockaddr*)&client_adr), s, sizeof(s));
        cout << "server: got connection from " << s << endl;

        thread clientThread(&Server::handleClient, this, clientSocket_fd);
        clientThread.detach(); // let the thread run, will use join later with thread pool
    }
}

/**
 * Sends data to a client socket then closes it.
 * @param csocket_fd Socket file descriptor of a client socket
 * @result If sending data is a success, the message is sent. If there is an error, 
 *         the error msg is printed. Closes the socket upon success and failure.
 */
void Server::handleClient(int csocket_fd) {
    if(send(csocket_fd, "Hello World!\n", 13, 0) == SEND_ERR) {
        perror("sever: send");
    }

    close(csocket_fd);
}

/* Non-member functions */

/**
 * Gets the IP address ptr from a sockaddr struct.
 * @param sa Ptr to a sockaddr struct
 * @return Ptr to the sin_addr (IPv4) or sin6_addr (IPv6) field
 */
void* get_in_addr(struct sockaddr* sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
