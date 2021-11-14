#include "Client.h"

int onUrl(http_parser *parser, const char *at, size_t length) {
    //todo rewrite me
    auto client = (Client *) parser->data;
    client->setIsRequestParsed(false);
    if (1u != parser->method && 2u != parser->method) {
        client->getLogger().debug(client->getTag(), "onUrl failed");
        return 1;
    }
    //todo maybe here
    auto tmp = std::string(at);
    client->url.append(at, length);
    client->getLogger().debug(client->getTag(), "URL=" + client->url);
    client->getLogger().debug(client->getTag(), "onUrl successful");

    return 0;
}

int onHeaderField(http_parser *parser, const char *at, size_t length) {
    auto client = (Client *) parser->data;
    client->h_field = std::string(at, length);
    return 0;
}

int onHeaderValue(http_parser *parser, const char *at, size_t length) {
    auto client = (Client *) parser->data;
    auto value = std::string(at, length);
    if (client->h_field == "Host") {
        if (!client->createServerConnection(value)) {
            return 1;
        }
    }
    if ("Connection" == client->h_field) {
        value = "close";
    }
    client->headers.append(client->h_field + ": " + value + "\r\n");
    client->getLogger().debug(client->getTag(), "Parsed header " + client->h_field + ": " + value);
    return 0;
}

int onHeadersComplete(http_parser *parser) {
    //todo rewrite me
    auto client = (Client *) parser->data;
    client->headers.append("\r\n");
    client->getLogger().debug(client->getTag(), "All headers parsed");
    switch (parser->method) {
        case 1u:
            client->method = "GET";
            break;
        case 2u:
            client->method = "HEAD";
            break;
        default:
            return 1;
    }
    client->sendServerRequest();
    client->setIsRequestParsed(true);
    return 0;
}

Client::Client(int client_socket, bool is_debug, Proxy *proxy) : logger(*(new Logger(is_debug))) {
    ///-------------------Initializing fields-------------------
    this->client_socket = client_socket;
    this->proxy = proxy;
    TAG = std::string("Client " + std::to_string(client_socket));

    ///---------Initializing parser settings---------
    http_parser_settings_init(&settings);
    settings.on_url = onUrl;
    settings.on_header_field = onHeaderField;
    settings.on_header_value = onHeaderValue;
    settings.on_headers_complete = onHeadersComplete;

    ///----------------Initializing parser----------------
    http_parser_init(&parser, HTTP_REQUEST);
    parser.data = this;

    logger.debug(TAG, "created and initialized");
}

bool Client::execute(int event) {
    //todo rewrite me
//    if (is_request_parsed) {
//        return readAnswer();
//    } else {
//        return readRequest();
//    }

    if (event & POLLIN) {
        logger.debug(TAG, "EXECUTE POLLIN" + std::to_string(event | POLLIN));
        return readRequest();
    }

    if (event & POLLOUT) {
        logger.debug(TAG, "EXECUTE POLLOUT" + std::to_string(event | POLLOUT));
        return readAnswer();
    }

    return false;
}

bool Client::readRequest() {
    char buffer[BUFSIZ];
    auto len = recv(client_socket, buffer, BUFSIZ, 0);
    //todo >
    if (0 >= len) {
        logger.debug(TAG, "len from recv <= 0");
        return false;
    }
    auto parsed_len = http_parser_execute(&parser, &settings, buffer, len);
    if (parsed_len != len || 0u != parser.http_errno) {
        logger.debug(TAG, "parser errno = " + std::to_string(parser.http_errno));
        return false;
    }

    return true;
}

bool Client::readAnswer() {
    logger.info(TAG, "READING FROM CACHE");
    if (nullptr == cached_data) {
        logger.info(TAG, "BROKEN CACHE");
        return false;
    }
    auto cache_len = cached_data->getRecordSize();
    size_t read_len;
    if (cache_len - current_pos > BUFSIZ) {
        read_len = BUFSIZ;
    } else {
        read_len = cache_len - current_pos;
    }
    auto data = cached_data->getPart(current_pos, read_len);
    current_pos += read_len;
    ssize_t bytes_sent = 0;
    while (bytes_sent != read_len) {
        ssize_t sent = send(client_socket, data.data() + bytes_sent, read_len, 0);
        if (0 > sent) {
            return false;
        }
        if (0 == sent) {
            break;
        }
        bytes_sent += sent;
    }

    if (cached_data->isFull() && current_pos == cached_data->getRecordSize()) {
        return false;
    }

    return true;
}

bool Client::createServerConnection(const std::string &host) {
    return proxy->createServerConnection(host, this);
}

void Client::addServer(Server *ser) {
    this->server = ser;
}

void Client::sendServerRequest() {
    server->sendRequest(url.data(), headers.data(), method.data());
}

void Client::setIsRequestParsed(bool isRequestParsed) {
    is_request_parsed = isRequestParsed;
}

void Client::addCache(CacheEntity *cache) {
    cached_data = cache;
    logger.debug(TAG, "Cache added");
}