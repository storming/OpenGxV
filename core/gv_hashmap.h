#ifndef __GV_HASHMAP_H__
#define __GV_HASHMAP_H__

#include "gv_platform.h"
#include "gv_hash.h"
#include "gv_object.h"
#include "gv_memory.h"

GV_NS_BEGIN

template <typename _Key, typename _T>
struct equal {
    bool operator()(const _Key &lhs, const _T &rhs) noexcept {
        return lhs == rhs;
    }
};

struct hashmap_entry : public list_entry {
    hash_t _hash;
};

template <
    typename _Key,
    typename _T,
    typename _Entry,
    _Entry _T::*__field,
    typename _Hash = hash<_Key>,
    typename _Compare = equal<_Key, _T>>
class hashmap {
public:
    typedef _T                              type;
    typedef _Key                            key_type;
    typedef _Entry                          entry_type;
    typedef _Hash                           hash_type;
    typedef _Compare                        compare_type;
    typedef bsdlist<_T, 
        typename member_of<
            decltype(__field)
        >::class_type,
        entry_type, 
        __field, 
        entry_type::list_type,
        is_ptr<_T>::value>                  list_type;
    typedef typename list_type::pointer     pointer;
    typedef typename list_type::iterator    iterator;
private:
    static entry_type &get_entry(type *elm) noexcept {
        return elm->*__field;
    }
    unsigned get_bucket(hash_t h) noexcept {
        return h & _capacity;
    }
    unsigned get_bucket(type *elm) noexcept {
        return get_bucket(get_entry(elm)._hash);
    }
    type *get_next(unsigned bucket, type *elm) noexcept {
        type* next = list_type::next(elm);
        if (next && get_bucket(get_entry(elm)._hash) != bucket) {
            next = nullptr;
        }
        return next;
    }
    bool equal(hash_t hash, const key_type &key, type *elm) noexcept {
        return hash == get_entry(elm)._hash && _compare(key, *elm);
    }
    pointer link(type **head, const pointer &elm) noexcept {
        if (*head) {
            list_type::insert_front(*head, elm);
        }
        else {
            _list.push_front(elm);
        }
        *head = elm;
        return elm;
    }
    pointer unlink(unsigned bucket, type**head, type *elm) noexcept {
        --_size;
        if (*head == elm) {
            *head = get_next(bucket, elm);
        }
        return list_type::remove(elm);
    }
public:
    hashmap(size_t capacity = 0, 
            hash_type hash = hash_type(), 
            compare_type compare = compare_type()) noexcept 
    : _hash(hash),
      _compare(compare),
      _map(),
      _size(),
      _capacity(3) 
    {
        expand();
    }

    ~hashmap() noexcept {
        mem_free(_map, sizeof(type*) * (_capacity + 1));
    }
    size_t size() const noexcept {
        return _size;
    }
    size_t capacity() const noexcept {
        return _capacity;
    }

    void expand() noexcept {
        if (_map) {
            mem_free(_map, sizeof(type*) * (_capacity + 1));
            _capacity = ((_capacity + 1) << 1) - 1;
        }
        _map = (type**)mem_alloc(sizeof(type*) * (_capacity + 1));
        list_type list;
        list = std::move(_list);
        pointer elm;
        while ((elm = list.pop_front())) {
            type **head = _map + get_bucket(elm);
            link(head, elm);
        }
    }

    template <typename _Constructor, typename ..._Args>
    std::pair<pointer, bool> emplace(const key_type &key, _Constructor constructor, _Args&&...args) {
        hash_t hash = _hash(key);
        unsigned bucket = get_bucket(hash);

        type **head = _map + bucket;
        type *elm = *head;
        while (elm) {
            if (equal(hash, key, elm)) {
                return std::pair<type*, bool>(elm, false);
            }
            elm = get_next(bucket, elm);
        }

        ++_size;
        return std::pair<pointer, bool>(link(head, constructor(std::forward<_Args>(args)...)), true);
    }

    type* find(const key_type &key) noexcept {
        hash_t hash = _hash(key);
        unsigned bucket = get_bucket(hash);

        type *elm = *(_map + bucket);
        while (elm) {
            if (equal(hash, key, elm)) {
                break;
            }
            elm = get_next(bucket, elm);
        }
        return elm;
    }

    pointer erase(type *elm) noexcept {
        hash_t hash = get_entry(elm)._hash;
        unsigned bucket = get_bucket(hash);
        return unlink(bucket, _map + bucket, elm);
    }

    pointer erase(const key_type &key) noexcept {
        hash_t hash = _hash(key);
        unsigned bucket = hash & _capacity;

        type **head = _map + bucket;
        type *elm = *head;
        while (elm) {
            if (equal(hash, key, elm)) {
                return unlink(bucket, head, elm);
            }
            elm = get_next(bucket, elm);
        }
        return nullptr;
    }

    pointer replace(const key_type &key, const pointer &new_elm) noexcept {
        hash_t hash = _hash(key);
        unsigned bucket = hash & _capacity;

        type **head = _map + bucket;
        type *elm = *head;
        while (elm) {
            if (equal(hash, key, elm)) {
                break;
            }
            elm = get_next(bucket, elm);
        }

        pointer ret(nullptr);
        if (elm) {
            ret = unlink(bucket, head, elm);
        }

        ++_size;
        link(head, new_elm);
        return ret;
    }
protected:
    list_type       _list;
    hash_type       _hash;
    compare_type    _compare;
    type          **_map;
    size_t          _size;
    size_t          _capacity;
}; 

GV_NS_END

#define gv_hashmap(_Key, _T, entry,...) GV_NS::hashmap<          \
    _Key,                                                        \
    _T,                                                          \
    typename GV_NS::member_of<decltype(&_T::entry)>::type,       \
    &_T::entry, ##__VA_ARGS__>

#endif
