#include "Cache.h"

/**
 * @return On success - Entity, otherwise - nullptr
 **/
CacheEntity *Cache::getEntity(const std::string &url) {
    try {
        return cached_data.at(url);
    } catch (std::out_of_range &exc) {
        return nullptr;
    }
}

CacheEntity *Cache::createEntity(const std::string &url) {
    try {
        return cached_data.insert(std::make_pair(url, new CacheEntity(url, logger->isDebug(), proxy))).first->second;
    } catch (std::exception &exc) {
        logger->info(TAG, "Can't create Entity for " + url);
        return nullptr;
    }
}

Cache::Cache(bool is_debug, Proxy *proxy1) : logger(new Logger(is_debug)) {
    this->proxy = proxy1;
    this->TAG = std::string("Cache");
    logger->debug(TAG, "created");
}

Cache::~Cache() {
    for (auto &item: cached_data) {
        delete item.second;
    }
    delete &logger;
}
