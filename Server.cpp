#include "Server.h"

bool Server::execute(int event) {
    char buffer[BUFSIZ];
    logger.debug(TAG, "EXECUTE");
    auto len = recv(server_socket, buffer, BUFSIZ, 0);
    if (len <= 0) {
        return false;
    }
    //std::cout << "Received " << len << " bytes" << std::endl;
    //std::cout << "Got answer" << std::endl << buffer << std::endl;
    //std::cout.write(buffer, len);
    //write(client_soc, buffer, len);
    logger.info(TAG, "GOT ANSWER, len = " + std::to_string(len));

    auto cache = proxy->getCache()->getEntity(url);
    auto data = std::string(buffer, len);
    if (cache != nullptr) {

        if (!is_client_subscribed) {
            cache->subscribe(client_soc);
            proxy->addCacheToClient(client_soc, cache);
            is_client_subscribed = true;
        }

        if (!cache->isFull()) {
            cache->expandData(data);
        } else {
            //todo do smth if cache is full
            cache->subscribe(client_soc);
            proxy->addCacheToClient(client_soc, cache);
            return false;
        }
    } else {
        logger.debug(TAG, "1");
        cache = proxy->getCache()->createEntity(url);
        logger.debug(TAG, "2");
        if (nullptr == cache) {
            return false;
        }

        if (!is_client_subscribed) {
            cache->subscribe(client_soc);
            proxy->addCacheToClient(client_soc, cache);
            is_client_subscribed = true;
        }

        logger.debug(TAG, "3");
        cache->expandData(data);
        logger.debug(TAG, "4");
    }

    /*ssize_t bytes_sent = 0;
    while (bytes_sent != len) {
        ssize_t sent = send(client_soc, buffer + bytes_sent, len, 0);
        if (0 > sent) {
            return false;
        }
        if (0 == sent) {
            break;
        }
        bytes_sent += sent;
    }*/

    logger.debug(TAG, "Answer sent to client " + std::to_string(client_soc));

    return true;
}

Server::Server(int server_socket, bool is_debug, Proxy *proxy) : logger(*(new Logger(is_debug))), proxy(proxy) {
    this->server_socket = server_socket;
    TAG = std::string("SERVER " + std::to_string(server_socket));
    logger.debug(TAG, "created");
}

void Server::sendRequest(const char *url1, const char *headers, const char *method) {
    auto get_this = std::string(method);
    get_this.append(" ");
    get_this.append(url);
    get_this.append(" HTTP/1.0\r\n");
    auto _headers = std::string(headers);
    get_this.append(_headers);
    //write(server_socket, get_this.data(), strlen(get_this.data()));
    //write(server_socket, headers, strlen(headers));
    send(server_socket, get_this.data(), get_this.size(), 0);
    //send(server_socket, headers, strlen(headers), 0);
    this->url = std::string(url);
    std::cout << "server sent request:\n" << get_this << "\n" << headers << std::endl;
}
