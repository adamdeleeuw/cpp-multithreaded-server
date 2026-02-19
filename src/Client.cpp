#include "../include/Client.h"
using namespace std;

Client::Client() {
    Setup();
    Run();
}

Client::~Client() {
    if (serverinfo != NULL) freeaddrinfo(serverinfo);
    if (socket_fd != SOCK_ERR) close(socket_fd);
}

void Client::Run() {
    Connect();
}

/* Private helper functions */

void Client::Setup() {
    setaddrinfo();

    status = getaddrinfo(host, PORT, &hints, &serverinfo); // get all required info
    if (status != 0) {
        throw runtime_error("getaddrinfo() errror: " + string(gai_strerror(status)));
    }
}

void Client::setaddrinfo() {
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
}

/**
 * @result looks for suitable hosts awating connection, and makes a connection if possible.
 */
void Client::Connect() {
    cout << "Looking for available hosts..." << endl;
    struct addrinfo* cr; // current result in the linked list

    for (cr = serverinfo; cr != nullptr; cr = cr->ai_next) {
        socket_fd = socket(cr->ai_family, cr->ai_socktype, cr->ai_protocol);

        if (socket_fd == SOCK_ERR) {
            perror("client: socket");
            continue;
        }

        inet_ntop(cr->ai_family, get_in_addr((struct sockaddr*)cr->ai_addr), s, sizeof(s)); // nail down understanding of this
        cout << "client: trying to connect to host at " << s << endl;

        if (connect(socket_fd, cr->ai_addr, cr->ai_addrlen) == CONCT_ERR) {
            perror("client: connect");
            close(socket_fd);
            socket_fd = SOCK_ERR;

            continue; // this socket couldn't connect to host, try next
        }

        break;
    }

    if (cr == nullptr) {
        throw runtime_error("client: client failed to connect");
    }

    // else, connect client (seems like duplicated logic?)
    inet_ntop(cr->ai_family, get_in_addr((struct sockaddr*)cr->ai_addr), s, sizeof(s)); // nail down understanding of this
    cout << "client: client connected to " << s << endl;

    freeaddrinfo(serverinfo);
    serverinfo = nullptr;

    Receive();
}  

void Client::Receive() {
    nbytes = recv(socket_fd, buf, MAX_DATASIZE, 0);
    if (nbytes == RECV_ERR) {
        perror("client: receive");
        exit(1); // end child process with error code
    }

    if (nbytes == 0) {
        cout << "client: connection closed by server" << endl;
    } else{
         buf[nbytes] = '\0';
        cout << "client: received " << buf << endl;
    }

    cout << "Work done, connection closing" << endl;
    
    close(socket_fd);
    socket_fd = SOCK_ERR;

    exit(0); // end child process with success
}

/* Non-member functions */

void* get_in_addr(struct sockaddr* sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}