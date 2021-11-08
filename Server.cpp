#include "Server.h"

void Server::execute(int event) {

}

Server::Server(int server_socket, bool is_debug, Proxy* proxy) : logger(*(new Logger(is_debug))), proxy(proxy) {
    this->server_socket = server_socket;
    TAG = std::string("SERVER " + std::to_string(server_socket));
    logger.debug(TAG, "created");
}
