#include <iostream>
#include "Server.h"
using namespace std;

/**
 * @effects creates a new HTTP server.
 * 
 * Note: 
 * Only a port is provided b/c we use the AI_PASSIVE flag to tell Linux not to discrimminate against any IP addresses.
 * Basically, we tell our OS to listen on all IP adrs.
 */
int main() {
    Server newServer;

    if (newServer.start("5000") == true) {
        newServer.run();
    }

    /* Progress note: start() and run() are just stubs to help me gather an idea of what data and methods I need
       for my constructor */
}
