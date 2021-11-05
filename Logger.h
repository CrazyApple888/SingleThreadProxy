#ifndef SINGLETHREADPROXY_LOGGER_H
#define SINGLETHREADPROXY_LOGGER_H

#include <iostream>

class Logger {
private:
    const bool isDebug;
    std::ostream &ostream;
public:
    explicit Logger(bool isDebug, std::ostream &ostream);

    void info(const std::string& tag, const std::string& message);

    void debug(const std::string &tag, const std::string &message);
};


#endif //SINGLETHREADPROXY_LOGGER_H
