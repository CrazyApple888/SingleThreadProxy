#ifndef SINGLETHREADPROXY_CLIENT_H
#define SINGLETHREADPROXY_CLIENT_H


#include <string>
#include "Handler.h"
#include "Logger.h"
#include "Proxy.h"
#include "http_parser.h"
#include "Server.h"

class Proxy;
class Server;

class Client : public Handler {
private:
    int client_socket;
    std::string TAG;
    Logger logger;
    Proxy *proxy;
    http_parser_settings settings{};
    http_parser parser{};
    Server *server;
public:
    std::string url;

    std::string h_field = "";
    std::string headers = "";

    Client() = default;

    Client(int client_socket, bool is_debug, Proxy *proxy);

    Logger getLogger() { return logger; }

    std::string getTag() { return TAG; }

    bool execute(int event) override;

    bool createServerConnection(const std::string &host);

    void addServer(Server *ser);

    void sendServerRequest();
};


#endif //SINGLETHREADPROXY_CLIENT_H
