#include "Proxy.h"

Proxy::Proxy(bool is_debug) : logger(*(new Logger(is_debug))) {}

int Proxy::start(int port) {
    this->proxy_port = port;
    if (EXIT_FAILURE == initProxySocket()) {
        logger.info(TAG, "Can't init socket");
        return EXIT_FAILURE;
    }
    initProxyPollFd();

    while (poll(clientsPollFd.data(), clientsPollFd.size(), 100000) > 0) {
        if (POLLIN == clientsPollFd[0].revents) {
            clientsPollFd[0].revents = 0;
            acceptClient();
        }

        for (auto i = 1; i < clientsPollFd.size(); i++) {
            auto item = clientsPollFd[i];
            if (POLLIN == item.revents) {
                //TODO work with event
                testRead(item.fd);
                item.revents = 0;
            }
            if ((POLLHUP | POLLIN) == item.revents) {
                disconnectClient(item, i);
                item.revents = 0;
            }
        }
        logger.debug(TAG, "Poll iteration completed");
    }

    logger.info(TAG, "proxy finished");
    return EXIT_SUCCESS;
}

void Proxy::testRead(int fd) {
    char buffer[BUFSIZ];
    read(fd, buffer, BUFSIZ);
    logger.info(TAG, buffer);
}

void Proxy::disconnectClient(struct pollfd client, size_t index) {
    close(client.fd);
    auto iter = clientsPollFd.begin() + index;
    clientsPollFd.erase(iter);
    handlers.erase(client.fd);
    logger.info(TAG, "Disconnected client with descriptor " + std::to_string(client.fd));
}

void Proxy::acceptClient() {
    int client_socket;
    struct sockaddr_un clientAddress{};
    socklen_t len = sizeof(clientAddress);
    //todo may be made it nonblock
    if ((client_socket = accept(proxy_socket, (struct sockaddr *) &clientAddress, &len)) < 0) {
        logger.info(TAG, "Can't accept client");
        return;
    }
    auto client = new Client(client_socket, logger.isDebug());
    handlers.insert(std::make_pair(client_socket, client));
    initClientPollFd(client_socket);
    logger.info(TAG, "Accepted new client with descriptor " + std::to_string(client_socket));
}

void Proxy::initClientPollFd(int socket) {
    struct pollfd client{};
    client.fd = socket;
    client.events = POLLIN | POLLHUP;
    client.revents = 0;

    clientsPollFd.push_back(client);
}

void Proxy::initProxyPollFd() {
    struct pollfd serverFd{};
    serverFd.fd = proxy_socket;
    serverFd.events = POLLIN | POLLHUP;
    serverFd.revents = 0;

    clientsPollFd.push_back(serverFd);
}

int Proxy::initProxySocket() {
    proxy_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (-1 == proxy_socket) {
        logger.info(TAG, "Can't create socket");
        return EXIT_FAILURE;
    }

    struct sockaddr_in proxy_addr{};
    proxy_addr.sin_family = AF_INET;
    proxy_addr.sin_port = htons(proxy_port);
    proxy_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (-1 == bind(proxy_socket, (sockaddr *) (&proxy_addr), sizeof(proxy_addr))) {
        close(proxy_socket);
        logger.info(TAG, "Can't bind socket");
        return EXIT_FAILURE;
    }

    if (-1 == fcntl(proxy_socket, F_SETFL, O_NONBLOCK)) {
        close(proxy_socket);
        return EXIT_FAILURE;
    }

    if (-1 == listen(proxy_socket, backlog)) {
        close(proxy_socket);
        logger.info(TAG, "Can't listen socket");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

Proxy::~Proxy() {
    close(proxy_socket);
}