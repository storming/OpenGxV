#ifndef __GV_UNISTR_H__
#define __GV_UNISTR_H__

#include <unordered_set>
#include <string>

#include "gv_object.h"
#include "gv_hash.h"
#include "gv_singleton.h"

GV_NS_BEGIN

class UniStr final : public Object {
    friend class UniStrPool;
    GV_FRIEND_PTR();
private:
    struct hash {
        size_t operator()(const UniStr *x) const noexcept {
            return x->_hash;
        }
    };
    struct equal {
        bool operator()(const UniStr *x, const UniStr *y) const noexcept {
            return x->_size == y->_size && x->_hash == y->_hash && !std::memcmp(x->_string, y->_string, x->_size);
        }
    };
    typedef std::unordered_set<ptr<UniStr>, hash, equal> map_type;
    void release() noexcept;

    UniStr() {}
    ~UniStr() noexcept {
        if (_string) {
            std::free(_string);
        }
    }
public:
    const char *c_str() const noexcept {
        return _string;
    }
    size_t size() const {
        return _size;
    }
    size_t hash() const {
        return _hash;
    }
    operator const char*() const noexcept {
        return _string;
    }
private:
    char *_string;
    size_t _size;
    size_t _hash;
    map_type::iterator _it;
};

class UniStrPool : public Object, public singleton<UniStrPool> {
    friend class UniStr;
    GV_FRIEND_SINGLETON();
public:
    const ptr<UniStr> &get(const char *str, size_t size = 0) noexcept;
    const ptr<UniStr> &get(const std::string &str) noexcept {
        return get(str.c_str(), str.size());
    }
private:
    UniStrPool() {}
    UniStr::map_type _map;
};

inline void UniStr::release() noexcept {
    if (_string && ref() == 2) {
        Object::release();
        UniStrPool::instance()->_map.erase(_it);
    }
    else {
        Object::release();
    }
}

GV_NS_END

inline const GV_NS::ptr<GV_NS::UniStr> &gv_unistr(const char *str, size_t size = 0) noexcept {
    return GV_NS::UniStrPool::instance()->get(str, size);
}

inline const GV_NS::ptr<GV_NS::UniStr> &gv_unistr(const std::string &str) noexcept {
    return GV_NS::UniStrPool::instance()->get(str);
}

#endif
