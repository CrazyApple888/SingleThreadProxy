#include "Proxy.h"

Proxy::Proxy(bool is_debug) : logger(*(new Logger(is_debug))) {
    this->cache = new Cache(is_debug, this);
}

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
            logger.debug(TAG, "Proxy POLLIN " + std::to_string(clientsPollFd[0].revents));
            clientsPollFd[0].revents = 0;
            acceptClient();
        }

        ///Checking for new messages from clients
        auto size = clientsPollFd.size();
        for (auto i = 1; i < size; i++) {
            logger.debug(TAG, "Revent before: " + std::to_string(clientsPollFd[i].fd) + " " + std::to_string(clientsPollFd[i].revents));
            //todo maybe ==, maybe without POLLOUT
            if ((POLLIN | POLLOUT) & clientsPollFd[i].revents) {
                bool is_success = false;
                try {
                    is_success = handlers.at(clientsPollFd[i].fd)->execute(clientsPollFd[i].revents);
                } catch (std::out_of_range &exc) {
                    logger.info(TAG, exc.what());
                    return EXIT_FAILURE;
                }
                if (!is_success) {
                    logger.debug(TAG, "Execute isn't successful for" + std::to_string(clientsPollFd[i].fd));
                    //sendErrorMessage(item.fd, E405);
                    disconnectClient(clientsPollFd[i], i);
                    clientsPollFd[i].revents = 0;
                    size--;
                    continue;
                }

                if ((POLLHUP | POLLIN) == clientsPollFd[i].revents) {
                    logger.info(TAG, "POLLHUP | POLLIN " + std::to_string(clientsPollFd[i].revents));
                    disconnectClient(clientsPollFd[i], i);
                    size--;
                    //clientsPollFd[i].revents = 0;
                }

                clientsPollFd[i].revents = 0;
            }
/*            if ((POLLHUP | POLLIN) == clientsPollFd[i].revents) {
                logger.info(TAG, "POLLHUP | POLLIN " + std::to_string(clientsPollFd[i].revents));
                disconnectClient(clientsPollFd[i], i);
                clientsPollFd[i].revents = 0;
            }*/
            if ((POLLERR | POLLIN) == clientsPollFd[i].revents) {
                disconnectClient(clientsPollFd[i], i);
                clientsPollFd[i].revents = 0;
            }

            logger.debug(TAG, "Revent after: " + std::to_string(clientsPollFd[i].fd) + " " + std::to_string(clientsPollFd[i].revents));
            clientsPollFd[i].revents = 0;
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
    auto _client = handlers.at(client.fd);
    auto iter = clientsPollFd.begin() + index;
    handlers.erase(client.fd);
    clientsPollFd.erase(iter);
    close(client.fd);
    delete _client;
    logger.info(TAG, "Disconnected client with descriptor " + std::to_string(client.fd));
}

void Proxy::acceptClient() {
    int client_socket;
    struct sockaddr_un clientAddress{};
    socklen_t len = sizeof(clientAddress);
    //todo may be made it nonblock
    if ((client_socket = accept(proxy_socket, nullptr, nullptr)) < 0) {
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
    if ((soc = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        logger.info(TAG, "Can't create socket for host" + host);
        return false;
    }

    struct sockaddr_in sockaddrIn{};
    sockaddrIn.sin_family = AF_INET;
    sockaddrIn.sin_port = htons(80);
    sockaddrIn.sin_addr = *((struct in_addr *) hostinfo->h_addr);

    if (-1 == (connect(soc, (struct sockaddr *) &sockaddrIn, sizeof(sockaddrIn)))) {
        logger.info(TAG, "Can't create connection to " + host);
        return false;
    }


    initClientPollFd(soc);
    auto server = new Server(soc, logger.isDebug(), this);
    handlers.insert(std::make_pair(soc, server));

    client->addServer(server);
    server->client_soc = client->client_socket;

    logger.info(TAG, "Added server with descriptor " + std::to_string(soc));

    return true;
}

void Proxy::disconnectSocket(int soc) {
    struct pollfd client{};
    int index = -1;
    for (int i = 0; i < clientsPollFd.size(); ++i) {
        if (clientsPollFd[i].fd == soc) {
            client = clientsPollFd[i];
            index = i;
            break;
        }
    }
    if (index != -1) {
        disconnectClient(client, index);
    }
}

void Proxy::notify(int soc) {
    for(auto &item : clientsPollFd) {
        if (soc == item.fd) {
            item.events |= POLLOUT;
            break;
        }
    }
}

Cache *Proxy::getCache() {
    return cache;
}

void Proxy::addCacheToClient(int soc, CacheEntity *cache_entity) {
    logger.debug(TAG, "addCacheToClient");
    for(auto &item : clientsPollFd) {
        if (soc == item.fd) {
            logger.debug(TAG, "addCacheToClient" + std::string("SOC == FD"));
            try {
                logger.debug(TAG, "!");
                dynamic_cast<Client*>(handlers.at(soc))->addCache(cache_entity);
                logger.debug(TAG, "!!");
            } catch (...) {
                logger.debug(TAG, /*std::string(exc.what()) +*/ "SOCKET = " + std::to_string(soc));
                exit(EXIT_FAILURE);
            }
            break;
        }
    }
}

void Proxy::disablePollout(int soc) {
    for (auto &item : clientsPollFd) {
        if (soc == item.fd) {
            item.events &= (~POLLOUT);
            break;
        }
    }
}
