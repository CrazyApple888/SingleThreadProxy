//
// Created by Андрей Исаченко on 07.11.2021.
//

#ifndef SINGLETHREADPROXY_SERVER_H
#define SINGLETHREADPROXY_SERVER_H


#include <sys/socket.h>
#include "Handler.h"
#include "Logger.h"

class Server : public Handler {
private:
    int server_socket;
    struct sockaddr server_address;
    std::string TAG;
    Logger logger;
public:
    Server(int server_socket, struct sockaddr server_address, bool is_debug);
    void execute() override;
};


#endif //SINGLETHREADPROXY_SERVER_H
