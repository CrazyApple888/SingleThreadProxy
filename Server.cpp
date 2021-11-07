//
// Created by Андрей Исаченко on 07.11.2021.
//

#include "Server.h"

void Server::execute() {

}

Server::Server(int server_socket, struct sockaddr server_address, bool is_debug) : logger(*(new Logger(is_debug))) {
    this->server_socket = server_socket;
    this->server_address = server_address;
    TAG = std::string("SERVER " + std::to_string(server_socket));
    logger.debug(TAG, "created");
}
