#ifndef __GV_HASH_H__
#define __GV_HASH_H__

#include <cinttypes>
#include "gv_platform.h"

GV_NS_BEGIN

inline size_t hash(const void *key, unsigned int len, size_t initval = 0x01000193U) {
    static const std::uint64_t m = 0xc6a4a7935bd1e995ULL;
    static const std::uint64_t r = 47;
    register std::uint64_t h = ((std::uint64_t)initval) ^ (len * m);
    register const std::uint64_t *data = (const std::uint64_t*)key;
    register const std::uint64_t *end = data + (len / sizeof(std::uint64_t));

    register std::uint64_t k;
    while (data != end) {
        k = *data++;
        k *= m;
        k ^= k >> r;
        k *= m;
        h ^= k;
        h *= m;
    };

    register const unsigned char *data2 = (const unsigned char*)data;
    switch (len & (sizeof(std::uint64_t) - 1)) {
    case 7: h ^= (uint64_t)(data2[6]) << 48;
    case 6: h ^= (uint64_t)(data2[5]) << 40;
    case 5: h ^= (uint64_t)(data2[4]) << 32;
    case 4: h ^= (uint64_t)(data2[3]) << 24;
    case 3: h ^= (uint64_t)(data2[2]) << 16;
    case 2: h ^= (uint64_t)(data2[1]) << 8;
    case 1: h ^= (uint64_t)(data2[0]);
        h *= m;
    }
    h ^= h >> r;
    h *= m;
    h ^= h >> r;
    return (size_t)h;
}

GV_NS_END

#endif