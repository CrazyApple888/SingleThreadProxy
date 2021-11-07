//
// Created by Андрей Исаченко on 07.11.2021.
//

#include "Client.h"

void Client::execute() {

}

Client::Client(int client_socket, bool is_debug) : logger(*(new Logger(is_debug))) {
    this->client_socket = client_socket;
    TAG = std::string("Client " + std::to_string(client_socket));
    logger.debug(TAG, "created");
}
