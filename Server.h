#ifndef SINGLETHREADPROXY_SERVER_H
#define SINGLETHREADPROXY_SERVER_H


#include <sys/socket.h>
#include <string>
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
    std::string url;
    bool is_client_subscribed = false;
public:
    int client_soc;

    Server(int server_socket, bool is_debug, Proxy *proxy);

    bool execute(int event) override;

    void sendRequest(const char *url1, const char *headers, const char *method);
};


#endif //SINGLETHREADPROXY_SERVER_H
