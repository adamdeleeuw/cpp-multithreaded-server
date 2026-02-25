#ifndef LOG
#define LOG

#include <iostream>
#include <string>

enum class Log {
    INFO,
    WARN,
    ERROR
};

void logmsg(Log type, const std::string& msg);
void logmsg(Log type, const std::string& msg, int errcode);

#endif