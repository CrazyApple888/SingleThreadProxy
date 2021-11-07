//
// Created by Андрей Исаченко on 07.11.2021.
//

#ifndef SINGLETHREADPROXY_CLIENT_H
#define SINGLETHREADPROXY_CLIENT_H


#include <string>
#include "Handler.h"
#include "Logger.h"

class Client : public Handler {
private:
    int client_socket;
    std::string TAG;
    Logger logger;
public:
    Client(int client_socket, bool is_debug);
    void execute() override;
};


#endif //SINGLETHREADPROXY_CLIENT_H
