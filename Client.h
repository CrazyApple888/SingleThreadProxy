#ifndef SINGLETHREADPROXY_CLIENT_H
#define SINGLETHREADPROXY_CLIENT_H


#include <string>
#include "Handler.h"
#include "Logger.h"
#include "Proxy.h"
#include "http_parser.h"

class Client : public Handler {
private:
    int client_socket;
    std::string TAG;
    Logger logger;
    Proxy *proxy;
    http_parser_settings settings{};
    http_parser parser{};
public:
    bool is_error;
    std::string url;

    Client(int client_socket, bool is_debug, Proxy *proxy);

    Logger getLogger() { return logger; }

    std::string getTag() { return TAG; }

    void execute(int event) override;
};


#endif //SINGLETHREADPROXY_CLIENT_H
