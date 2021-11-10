#ifndef SINGLETHREADPROXY_SERVER_H
#define SINGLETHREADPROXY_SERVER_H


#include <sys/socket.h>
#include "Handler.h"
#include "Logger.h"
#include "Proxy.h"

class Proxy;

class Server : public Handler {
private:
    int server_socket;
    std::string TAG;
    Logger logger;
    Proxy *proxy;
public:
    Server(int server_socket, bool is_debug, Proxy* proxy);
    bool execute(int event) override;
    void sendRequest(const char *url, const char* headers) const;
};


#endif //SINGLETHREADPROXY_SERVER_H
