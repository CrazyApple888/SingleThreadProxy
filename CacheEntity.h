#ifndef SINGLETHREADPROXY_CACHEENTITY_H
#define SINGLETHREADPROXY_CACHEENTITY_H

#include <iostream>
#include <vector>

class CacheEntity {
private:
    std::vector<uint8_t> record;
    bool is_full = false;
public:
    std::vector<uint8_t> getPart(size_t part_size);
    size_t getRecordSize();
    bool isFull() const;
};


#endif //SINGLETHREADPROXY_CACHEENTITY_H
