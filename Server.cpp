#include "Server.h"

bool Server::execute(int event) {
    char buffer[BUFSIZ];
    logger.info(TAG, "EXECUTE");
    auto len = recv(server_socket, buffer, BUFSIZ, 0);
    if (len <= 0) {
        return false;
    }
    //std::cout << "Received " << len << " bytes" << std::endl;
    //std::cout << "Got answer" << std::endl << buffer << std::endl;
    logger.info(TAG, "GOT ANSWER, len = " + std::to_string(len));
    //std::cout.write(buffer, len);
    //write(client_soc, buffer, len);

    ssize_t bytes_sent = 0;
    while (bytes_sent != len) {
        ssize_t sent = send(client_soc, buffer, len, bytes_sent);
        bytes_sent += sent;
    }

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
    auto _headers = std::string(headers);
    get_this.append(_headers);
    //write(server_socket, get_this.data(), strlen(get_this.data()));
    //write(server_socket, headers, strlen(headers));
    send(server_socket, get_this.data(), get_this.size(), 0);
    //send(server_socket, headers, strlen(headers), 0);
    std::cout << "server sent request:\n" <<  get_this << "\n" << headers << std::endl;
}
