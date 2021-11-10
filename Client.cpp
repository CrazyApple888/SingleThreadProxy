#include "Client.h"

int onUrl(http_parser *parser, const char *at, size_t length) {
    //todo rewrite me
    auto client = (Client *) parser->data;
    std::cout << "ZDAROVA onUrl " << std::endl;
    if (1u != parser->method && 2u != parser->method) {
        client->getLogger().debug(client->getTag(), "onUrl failed");
        return 1;
    }
    client->url.append(at, length);
    client->getLogger().debug(client->getTag(), "onUrl successful");

    return 0;
}

int onHeaderField(http_parser *parser, const char *at, size_t length) {
    //todo rewrite me
    auto client = (Client *) parser->data;
    std::cout << "ZDAROVA onHeaderField " << std::string(at, length) << std::endl;
    client->h_field = std::string(at, length);
    return 0;
}

int onHeaderValue(http_parser *parser, const char *at, size_t length) {
    //todo rewrite me
    auto client = (Client *) parser->data;
    auto value = std::string(at, length);
    if (client->h_field == "Host") {
        client->createServerConnection(value);
    }
    if ("Connection" == client->h_field) {
        value = "close";
    }
    client->headers.append(client->h_field + ": " + value + "\r\n");
    std::cout << "ZDAROVA onHeaderValue " << value << std::endl;
    return 0;
}

int onHeadersComplete(http_parser *parser) {
    //todo rewrite me
    auto client = (Client *) parser->data;
    client->headers.append("\r\n");
    std::cout << "ZDAROVA onHeadersComplete " << client->headers << std::endl;
    client->sendServerRequest();
    return 0;
}

bool Client::execute(int event) {
    //todo rewrite me
    char buffer[BUFSIZ];
    auto len = recv(client_socket, buffer, BUFSIZ, 0);
    logger.info(TAG, buffer);
    if (0 >= len) {
        logger.debug(TAG, "len from recv < 0");
        return false;
    }
    auto parsed_len = http_parser_execute(&parser, &settings, buffer, len);
    if (parsed_len != len || 0u != parser.http_errno) {
        logger.debug(TAG, "parse errno = " + std::to_string(parser.http_errno));
        return false;
    }
    return true;
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

bool Client::createServerConnection(const std::string &host) {
    proxy->createServerConnection(host, this);
}

void Client::addServer(Server *ser) {
    this->server = ser;
}

void Client::sendServerRequest() {
    server->sendRequest(url.data(), headers.data());
}
