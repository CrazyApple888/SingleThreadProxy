#include "Client.h"

int onUrl(http_parser *parser, const char *at, size_t length) {
    //todo rewrite me
    auto client = (Client*)parser->data;
    std::cout << "ZDAROVA onUrl" << std::endl;
    if (1u != parser->method && 2u != parser->method) {
        client->is_error = true;
        return 1;
    }
    client->url.append(at, length);
    client->getLogger().info(client->getTag(), "ON URL SUCCESSFUL");
    return 0;
}

int onHeaderField(http_parser *parser, const char *at, size_t length) {
    //todo rewrite me
    std::cout << "ZDAROVA onHeaderField" << std::endl;
    return 0;
}

int onHeaderValue(http_parser *parser, const char *at, size_t length) {
    //todo rewrite me
    std::cout << "ZDAROVA onHeaderValue" << std::endl;
    return 0;
}

int onHeadersComplete(http_parser *parser) {
    //todo rewrite me
    std::cout << "ZDAROVA onHeadersComplete" << std::endl;
    return 0;
}

int onBody(http_parser *parser, const char *at, size_t length) {
    //todo rewrite me
    std::cout << "ZDAROVA onBody" << std::endl;
    return 0;
}

void Client::execute(int event) {
    //todo rewrite me
    char buffer[BUFSIZ];
    auto len = recv(client_socket, buffer, BUFSIZ, 0);
    http_parser_execute(&parser, &settings, buffer, len);
}

Client::Client(int client_socket, bool is_debug, Proxy* proxy) : logger(*(new Logger(is_debug))) {
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
    settings.on_body = onBody;

    ///----------------Initializing parser----------------
    http_parser_init(&parser, HTTP_REQUEST);
    parser.data = this;

    logger.debug(TAG, "created and initialized");
}