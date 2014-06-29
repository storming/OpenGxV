#ifndef __GV_HASHMAP_H__
#define __GV_HASHMAP_H__

#include "gv_platform.h"
#include "gv_hash.h"
#include "gv_list.h"
#include "gv_memory.h"

GV_NS_BEGIN

template <typename _Key, typename _T>
struct equal {
    bool operator()(const _Key &lhs, const _T &rhs) noexcept {
        return lhs == rhs;
    }
};

struct hashmap_entry : public list_entry {
    unsigned _hash;
};

template <
    typename _Key,
    typename _T,
    typename _U,
    typename _Entry,
    _Entry _U::*__field,
    typename _Hash,
    typename _Compare = equal<_Key, _T>>
class hashmap {
public:
    typedef _T                                  type;
    typedef _Key                                key_type;
    typedef _Entry                              entry_type;
    typedef _Hash                               hash_type;
    typedef _Compare                            compare_type;
    typedef list<type, entry_type, __field, _U> list_type;
    typedef typename list_type::iterator        iterator;
private:
    static entry_type &get_entry() noexcept {
        return elm->*__field;
    }
    unsigned get_bucket(unsigned h) noexcept {
        return h & _capacity;
    }
    unsigned get_bucket(type *elm) noexcept {
        return get_bucket(get_entry()._hash);
    }
    unsigned get_next(unsigned bucket, type *elm) noexcept {
        type* next = list_type::next(elm);
        if (next && get_bucket(get_entry(elm)._hash) != bucket) {
            next = nullptr;
        }
        return next;
    }
    bool equal(unsigned hash, const key_type &key, type *elm) noexcept {
        return hash == get_entry()._hash && _compare(key, *elm);
    }

    static void link(type **head, type *elm) noexcept {
        if (*head) {
            list_type::insert_front(*head, elm);
        }
        else {
            list.push_front(elm);
        }
        *head = elm;
    }
    void unlink(unsigned hash, unsigned bucket, type**head, type*elm) noexcept {
        if (*head == elm) {
            *head = get_next(hash, bucket, elm);
        }
        list_type::remove(elm);
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
        list_type list
        type *elm;
        while ((elm = _list->pop_front())) {
            type **head = _map + get_bucket(elm);
            link(head, elm);
        }
        _list = std::move(list);
    }

    template <typename _Constructor>
    std::pair<type*, bool> emplace(const key_type &key, _Constructor constructor) {
        unsigned hash = _hash(key);
        unsigned bucket = get_bucket(hash);

        type **head = _map + bucket;
        type *elm = *head;
        while (elm) {
            if (equal(hash, key, elm)) {
                return std::pair<type*, bool>(elm, false);
            }
            elm = get_next(bucket, elm);
        }

        elm = constructor();
        link(head, elm);
        ++_size;
        return std::pair<type*, bool>(elm, true);
    }

    type* find(const key_type &key) noexcept {
        unsigned hash = _hash(key);
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

    type *erase(type *elm) noexcept {
        unsigned hash = _hash(key);
        unsigned bucket = get_bucket(hash);
        unlink(hash, bucket, _map + bucket, elm);
        --_size;
        return elm;
    }

    type *erase(const key_type &key) noexcept {
        unsigned hash = _hash(key);
        unsigned bucket = hash & _capacity;

        type **head = _map + bucket;
        type *elm = *head;
        while (elm) {
            if (equal(hash, key, elm)) {
                unlink(hash, bucket, head, elm);
                --_size;
                return elm;
            }
            elm = get_next(bucket, elm);
        }
        return nullptr;
    }

    type *replace(const key_type &key, type *new_elm) noexcept {
        unsigned hash = _hash(key);
        unsigned bucket = hash & _capacity;

        type **head = _map + bucket;
        type *elm = *head;
        while (elm) {
            if (equal(hash, key, elm)) {
                break;
            }
            elm = get_next(bucket, elm);
        }

        if (elm) {
            unlink(hash, bucket, head, elm);
        }
        else {
            ++_size;
        }

        link(head, new_elm);
        return elm;
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



#endif
