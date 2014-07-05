#ifndef __GV_HASH_H__
#define __GV_HASH_H__

#include <cinttypes>
#include "gv_platform.h"

GV_NS_BEGIN

typedef unsigned hash_t;
typedef std::uint64_t hash64_t;

inline hash64_t hash64(const void *key, size_t len, hash64_t initval = 0x01000193U) noexcept {
    static const hash64_t m = 0xc6a4a7935bd1e995ULL;
    static const hash64_t r = 47;
    register hash64_t h = ((std::uint64_t)initval) ^ (len * m);
    register const hash64_t *data = (const std::uint64_t*)key;
    register const hash64_t *end = data + (len / sizeof(hash64_t));

    register hash64_t k;
    while (data != end) {
        k = *data++;
        k *= m;
        k ^= k >> r;
        k *= m;
        h ^= k;
        h *= m;
    };

    register const unsigned char *data2 = (const unsigned char*)data;
    switch (len & (sizeof(hash64_t) - 1)) {
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
    return h;
}

template <typename _T>
struct hash {
    hash_t operator()(const _T &obj) noexcept {
        return (hash_t)hash64(std::address_of<const _T>(obj), strlen(_T));
    }
};

#define GV_HASH_STR_THRESHOLD (sizeof(hash64_t) * 2)
inline hash_t hash_string(const char *p, size_t len) noexcept {
    hash64_t h;
    if (len > GV_HASH_STR_THRESHOLD) {
        h = hash64(p, GV_HASH_STR_THRESHOLD);
        size_t n =  len & (sizeof(hash64_t) - 1);
        h = hash64(p + len - n, n, h);
    }
    else {
        h = hash64(p, len);
    }
    return (hash_t)h;
}

GV_NS_END

#endif