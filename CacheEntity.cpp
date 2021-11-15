#include "CacheEntity.h"

std::string CacheEntity::getPart(unsigned long start, unsigned long length) {
    return data.substr(start, length);
}

bool CacheEntity::isFull() const {
    return is_full;
}

size_t CacheEntity::getRecordSize() {
    return data.size();
}

/**
 * @param newData - part of data
 * @return true on success, false on bad_alloc
 */
bool CacheEntity::expandData(std::string &newData) {
    try {
        data.append(newData);
        notifySubscribers();
        return true;
    } catch (std::bad_alloc &exc) {
        logger.info(TAG, "bad alloc");
        return false;
    }
}

CacheEntity::CacheEntity(const std::string &url, bool is_debug, Proxy *proxy1) : logger(*(new Logger(is_debug))),
                                                                                 proxy(proxy1) {
    this->TAG = std::string("CacheEntity ") + url;
    logger.debug(TAG, "created");
}

void CacheEntity::subscribe(int soc) {
    subscribers.push_back(soc);
    if (is_full) {
        notifySubscribers();
    }
}

void CacheEntity::notifySubscribers() {
    for (auto &item: subscribers) {
        proxy->notify(item);
    }
}

void CacheEntity::setFull() {
    is_in_process = false;
    is_full = true;
}

bool CacheEntity::isInProcess() const {
    return is_in_process;
}

void CacheEntity::unsubscribe(int soc) {
    auto iter = subscribers.begin();
    for (; iter != subscribers.end(); iter++) {
        if (soc == (*iter)) {
            subscribers.erase(iter);
            logger.debug(TAG, std::to_string(soc) + " is now unsub");
            break;
        }
    }
}
