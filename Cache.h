#ifndef SINGLETHREADPROXY_CACHE_H
#define SINGLETHREADPROXY_CACHE_H

#include <iostream>
#include <map>
#include "CacheEntity.h"

class Cache {
private:
    std::map<std::string, CacheEntity*> cached_data;
public:
    CacheEntity *getEntity(std::string url);

};


#endif //SINGLETHREADPROXY_CACHE_H
