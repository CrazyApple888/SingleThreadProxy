#ifndef SINGLETHREADPROXY_CLIENT_H
#define SINGLETHREADPROXY_CLIENT_H


#include <string>
#include "Handler.h"
#include "Logger.h"
#include "Proxy.h"
#include "http_parser.h"
#include "Server.h"
#include "CacheEntity.h"

class Proxy;
class Server;
class CacheEntity;

class Client : public Handler {
private:
    std::string TAG;
    Logger logger;
    Proxy *proxy;
    http_parser_settings settings{};
    http_parser parser{};
    Server *server;
    bool is_request_parsed = false;
    bool readRequest();
    bool readAnswer();
    CacheEntity *cached_data = nullptr;
    size_t current_pos = 0;
public:
    //todo make me private
    int client_socket;
    std::string url;
    std::string method;

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
    void setIsRequestParsed(bool isRequestParsed);
    void addCache(CacheEntity *cache);
};


#endif //SINGLETHREADPROXY_CLIENT_H
