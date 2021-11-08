#ifndef SINGLETHREADPROXY_SERVER_H
#define SINGLETHREADPROXY_SERVER_H


#include <sys/socket.h>
#include "Handler.h"
#include "Logger.h"
#include "Proxy.h"

class Server : public Handler {
private:
    int server_socket;
    std::string TAG;
    Logger logger;
    Proxy *proxy;
public:
    Server(int server_socket, bool is_debug, Proxy* proxy);
    void execute(int event) override;
};


#endif //SINGLETHREADPROXY_SERVER_H
