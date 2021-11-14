#ifndef SINGLETHREADPROXY_CACHEENTITY_H
#define SINGLETHREADPROXY_CACHEENTITY_H

#include <iostream>
#include <vector>

#include "Logger.h"
#include "Proxy.h"

class Proxy;

class CacheEntity {
private:
    std::string TAG;
    std::string data;
    bool is_full = false;
    Logger logger;
    Proxy *proxy;
    std::vector<int> subscribers;
    void notifySubscribers();
    bool is_in_process = true;
public:
    CacheEntity(const std::string &url, bool is_debug, Proxy *proxy1);
    std::string getPart(unsigned long start, unsigned long length);
    size_t getRecordSize();
    bool isFull() const;
    bool expandData(std::string &newData);
    void subscribe(int soc);
    void setFull();
    bool isInProcess() const;
};


#endif //SINGLETHREADPROXY_CACHEENTITY_H
