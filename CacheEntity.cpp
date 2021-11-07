#include "CacheEntity.h"

std::vector<uint8_t> CacheEntity::getPart(size_t part_size) {

}

bool CacheEntity::isFull() const {
    return is_full;
}

size_t CacheEntity::getRecordSize() {
    return record.size();
}
