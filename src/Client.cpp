#include "../include/Client.h"
#include "../include/log.h"
#include "../include/server_exceptions.h"
using namespace std;

/**
 * @result Sets up a new client and initiates connection to server
 */
Client::Client() : host(IP), serverinfo(nullptr), socket_fd(SOCK_ERR) {
    Setup();
    Run();
}

/**
 * @result Closes socket and cleans up address info data.
 */
Client::~Client() {
    if (serverinfo != NULL) freeaddrinfo(serverinfo);
    if (socket_fd != SOCK_ERR) close(socket_fd);
}

/**
 * Client "play button".
 * @result Initiates connection to server
 */
void Client::Run() {
    Connect();
}

/**
 * Sets necessary address info and retrieves server details.
 * @result Populates serverinfo with available hosts matching the specified port.
 * @throws If getaddrinfo() fails
 */
void Client::Setup() {
    setaddrinfo();

    status = getaddrinfo(host, PORT, &hints, &serverinfo); // get all required info
    if (status != 0) {
        throw SetupException("getaddrinfo() error: " + string(gai_strerror(status)));
    }
}

/**
 * Initializes address info hints structure with default TCP settings.
 * @result Fills hints struct for IPvX (4/6 - either or) TCP stream socket
 */
void Client::setaddrinfo() {
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
}

/**
 * Looks for a suitable server to connect to from the serverinfo list.
 * @result Creates a socket and connects to the first reachable server. Gets and displays the server adr.
 * @throws If no connection can be made to any hosts in serverinfo.
 */
void Client::Connect() {
    logmsg(Log::INFO, "Looking for available hosts...");
    struct addrinfo* cr; // current result in the linked list

    for (cr = serverinfo; cr != nullptr; cr = cr->ai_next) {
        socket_fd = socket(cr->ai_family, cr->ai_socktype, cr->ai_protocol);

        if (socket_fd == SOCK_ERR) {
            logmsg(Log::ERROR, "client: socket", errno);
            continue;
        }

        // display host we are attempting to connect to
        inet_ntop(cr->ai_family, get_in_addr((struct sockaddr*)cr->ai_addr), s, sizeof(s));
        logmsg(Log::INFO, "client: trying to connect to host at " + string(s));

        if (connect(socket_fd, cr->ai_addr, cr->ai_addrlen) == CONCT_ERR) {
            logmsg(Log::ERROR, "client: connect", errno);
            close(socket_fd);
            socket_fd = SOCK_ERR;

            continue; // this socket couldn't connect to host, try next
        }

        break;
    }

    if (cr == nullptr) {
        throw SetupException("client: failed to connect to any host");
    }

    // display the host we ACTUALLY connected to
    inet_ntop(cr->ai_family, get_in_addr((struct sockaddr*)cr->ai_addr), s, sizeof(s));
    logmsg(Log::INFO, "client: connected to " + string(s));

    freeaddrinfo(serverinfo);
    serverinfo = nullptr;

    Receive();
}  

/**
 * Receives data from server and displays it.
 * @result Reads message from server socket, prints it, then closes and exits (kills client process)
 */
void Client::Receive() {
    nbytes = recv(socket_fd, buf, MAX_DATASIZE, 0);
    if (nbytes == RECV_ERR) {
        logmsg(Log::ERROR, "client: receive", errno);
        exit(1); // end child process with error code
    }

    if (nbytes == 0) {
        logmsg(Log::WARN, "client: connection closed by server");
    } else{
        buf[nbytes] = '\0';
        logmsg(Log::INFO, "client: received " + string(buf));
    }

    logmsg(Log::INFO, "Work done, connection closing");
    
    close(socket_fd);
    socket_fd = SOCK_ERR;

    exit(0); // end child process with success
}


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