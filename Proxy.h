#ifndef SINGLETHREADPROXY_PROXY_H
#define SINGLETHREADPROXY_PROXY_H


#include <cstdlib>
#include <poll.h>
#include <fcntl.h>
#include <cstdio>
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <set>
#include <map>
#include <vector>
#include <netdb.h>
#include <sys/un.h>
#include <algorithm>

#include "Logger.h"
#include "Handler.h"
#include "Client.h"
//#include "Server.h"

enum HTTP_ERROR{
    E405
};

class Client;

class Proxy {
private:
    const char* error_message_405 =  "HTTP/1.0 405 METHOD NOT ALLOWED\r\n\r\n Method Not Allowed";
    Logger logger;
    const std::string TAG = "PROXY";
    const int backlog = 20;
    int proxy_port;
    int proxy_socket;
    std::vector<struct pollfd> clientsPollFd;
    std::map<int, Handler*> handlers;

    int initProxySocket();
    void initProxyPollFd();
    void acceptClient();
    void initClientPollFd(int socket);
    void testRead(int fd);
    void disconnectClient(pollfd client, size_t index);
    void sendErrorMessage(int socket, HTTP_ERROR type);
public:

    explicit Proxy(bool is_debug);

    ~Proxy();

    int start(int port);

    bool createServerConnection(const std::string& host, Client *client);

    void disconnectSocket(int soc);

};


#endif //SINGLETHREADPROXY_PROXY_H
