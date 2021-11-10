#include "Server.h"

bool Server::execute(int event) {
    char buffer[2 * BUFSIZ];

    read(server_socket, buffer, BUFSIZ);
    std::cout << buffer << std::endl;

    return true;
}

Server::Server(int server_socket, bool is_debug, Proxy* proxy) : logger(*(new Logger(is_debug))), proxy(proxy) {
    this->server_socket = server_socket;
    TAG = std::string("SERVER " + std::to_string(server_socket));
    logger.debug(TAG, "created");
}

void Server::sendRequest(const char *url, const char *headers) const {
    auto get_this = std::string("GET ");
    get_this.append(url);
    get_this.append(" HTTP/1.0\r\n");
    write(server_socket, get_this.data(), strlen(get_this.data()));
    write(server_socket, headers, strlen(headers));
    std::cout << "server send request:\n" <<  get_this << "\n" << headers << std::endl;
}
