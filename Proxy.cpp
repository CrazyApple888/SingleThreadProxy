//#include "Client.h"
//#include "Server.h"
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
        ///New client
        if (POLLIN == clientsPollFd[0].revents) {
            clientsPollFd[0].revents = 0;
            acceptClient();
        }

        ///Checking for new messages from clients
        for (auto i = 1; i < clientsPollFd.size(); i++) {
            auto item = clientsPollFd[i];

            if (POLLIN & item.revents) {
                bool is_success = false;
                try {
                    is_success = handlers.at(item.fd)->execute(item.revents);
                } catch (std::out_of_range &exc) {
                    logger.info(TAG, exc.what());
                    return EXIT_FAILURE;
                }
                if (!is_success) {
                    logger.debug(TAG, "Execute isn't successful for" + std::to_string(item.fd));
                    sendErrorMessage(item.fd, E405);
                    disconnectClient(item, i);
                    continue;
                }
                item.revents = 0;
            }

            if ((POLLHUP | POLLIN) & item.revents) {
                disconnectClient(item, i);
                item.revents = 0;
            }

            if ((POLLERR | POLLIN) & item.revents) {
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

void Proxy::sendErrorMessage(int socket, HTTP_ERROR type) {
    switch (type) {
        case HTTP_ERROR::E405: {
            write(socket, error_message_405, strlen(error_message_405));
            break;
        }
    }
}

void Proxy::disconnectClient(struct pollfd client, size_t index) {
    close(client.fd);
    auto _client = handlers.at(client.fd);
    auto iter = clientsPollFd.begin() + index;
    clientsPollFd.erase(iter);
    handlers.erase(client.fd);
    delete _client;
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
    auto client = new Client(client_socket, logger.isDebug(), this);
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

bool Proxy::createServerConnection(const std::string &host, Client *client) {
    logger.info(TAG, host);
    struct hostent *hostinfo = gethostbyname(host.data());
    if (nullptr == hostinfo) {
        return false;
    }

    int soc;
    ///PF_INET IPPROTO_TCP
    if ((soc = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Error in socket\n");
        return false;
    }

    struct sockaddr_in sockaddrIn{};
    sockaddrIn.sin_family = AF_INET;
    sockaddrIn.sin_port = htons(80);
    sockaddrIn.sin_addr = *((struct in_addr *) hostinfo->h_addr);

    if (-1 == (connect(soc, (struct sockaddr *) &sockaddrIn, sizeof(sockaddrIn)))) {
        return false;
    }


    initClientPollFd(soc);
    auto server = new Server(soc, logger.isDebug(), this);
    handlers.insert(std::make_pair(soc, server));

    client->addServer(server);

    logger.info(TAG, "Added server with descriptor " + std::to_string(soc));

    return true;
}
