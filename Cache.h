#ifndef SINGLETHREADPROXY_CACHE_H
#define SINGLETHREADPROXY_CACHE_H

#include <iostream>
#include <map>
#include "CacheEntity.h"

class Cache {
private:
    std::map<std::string, CacheEntity*> cached_data;
    Logger logger;
    std::string TAG;
    Proxy *proxy;
public:

    Cache(bool is_debug, Proxy *proxy1);

    CacheEntity *getEntity(std::string url);

    bool contains(const std::string &url);

    CacheEntity *createEntity(const std::string &url);

};


#endif //SINGLETHREADPROXY_CACHE_H
