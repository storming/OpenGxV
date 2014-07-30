#ifndef __GV_UNISTR_H__
#define __GV_UNISTR_H__

#include <unordered_set>
#include <string>
#include <cstdlib>

#include "gv_object.h"
#include "gv_hash.h"
#include "gv_hashmap.h"
#include "gv_singleton.h"
#include "gv_chunk.h"

GV_NS_BEGIN

class UniStr : public Object {
    friend class UniStrPool;
    friend class Object;
private:
    UniStr(const char *str, size_t size) noexcept;
    ~UniStr() noexcept;
    hashmap_entry _entry;
public:
    const char *c_str() const noexcept {
        return (const char*)_data;
    }
    size_t size() const {
        return _size;
    }
    size_t hash() const {
        return _entry._hash;
    }
    operator const char*() const noexcept {
        return (const char*)_data;
    }
private:
    char  *_data;
    size_t _size;
}; 

class UniStrPool : public Object, public singleton<UniStrPool> {
    friend class UniStr;
    friend class Object;
public:
    ptr<UniStr> get(const char *str, size_t size = 0) noexcept;
    ptr<UniStr> get(const std::string &str) noexcept {
        return get(str.c_str(), str.size());
    }
private:
    typedef std::pair<const char*, size_t> key_type;
    struct hash {
        hash_t operator()(const key_type &key) noexcept {
            return hash_string(key.first, key.second);
        }
    };
    struct equal {
        bool operator()(const key_type &key, const UniStr &str) noexcept {
            return key.second == str.size() && !memcmp(key.first, str.c_str(), key.second);
        }
    };
    typedef gv_hashmap(key_type, UniStr, _entry, hash, equal) map_type;

    UniStrPool() {}
    map_type _map;
};

inline ptr<UniStr> unistr(const char *str, size_t size = 0) noexcept {
    return UniStrPool::instance()->get(str, size);
}

inline ptr<UniStr> unistr(const std::string &str) noexcept {
    return UniStrPool::instance()->get(str);
}

GV_NS_END

#define GV_STATIC_UNISTR2(name, str)                                  \
static class __GV_STATIC_UNISTR_##name {                              \
    struct impl : GV_NS::Object, GV_NS::singleton<impl, UniStrPool> { \
        bool init() {                                                 \
            _str = GV_NS::UniStrPool::instance()->get(#str);          \
            return true;                                              \
        }                                                             \
        GV_NS::ptr<GV_NS::UniStr> _str;                               \
    };                                                                \
public:                                                               \
    operator const GV_NS::ptr<GV_NS::UniStr>&() const noexcept {      \
        return impl::instance()->_str;                                \
    }                                                                 \
} name

#define GV_STATIC_UNISTR(name)                                        \
GV_STATIC_UNISTR2(name, #name)

#define GV_IMPL_UNISTR(CLASS, name)                                   \
decltype(CLASS::name) CLASS::name

#endif
