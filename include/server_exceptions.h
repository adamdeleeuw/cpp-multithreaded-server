#ifndef SERVER_EXCEPTIONS
#define SERVER_EXCEPTIONS

#include <stdexcept>
#include <string>
#include <cerrno>
#include <cstring>

using namespace std;

// extend runtime_error
class ServerException : public runtime_error {
    public:
        explicit ServerException(const string& msg) : runtime_error(msg), errcode(errno) {};
        int getErrno() const {return errcode; }
    
        private:
            int errcode;
};

class SetupException : public ServerException {
    public:
        explicit SetupException(const string& msg) : ServerException("Setup failed:" + msg) {}
};

// will add exceptions specific to clients, listener (network activities like accept())

#endif