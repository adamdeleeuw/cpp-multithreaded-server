#include "../include/log.h"
#include <cstring>
#include <cerrno>
using namespace std;

void logmsg(Log type, const string& msg) {
    switch(type) {
        case Log::INFO:
            cout << "INFO: " << msg << endl;
            break;
        case Log::WARN:
            cout << "WARN: " << msg << endl;
            break;
        case Log::ERROR:
            cerr << "ERROR: " << msg << endl;
            break;
    }
}

void logmsg(Log type, const string& msg, int errcode) {
    switch(type) {
        case Log::INFO:
            cout << "INFO: " << msg << endl;
            break;
        case Log::WARN:
            cout << "WARN: " << msg << endl;
            break;
        case Log::ERROR:
            cerr << "ERROR: " << msg << " | " << string(strerror(errcode)) << endl;
            break;
    }
}