#ifndef __GV_LIST_H__
#define __GV_LIST_H__

#include "gv_platform.h"

GV_NS_BEGIN

#define __LIST_OBJECT__(x)   static_cast<type*>(x)
#define __LIST_ENTRY__(x)    static_cast<entry_type&>(__LIST_OBJECT__(x)->*(__field))
#define __LIST_VALUE__(x)    static_cast<value_type*>(x)

struct list_type {
    enum {
        list,
        clist,
        slist,
        stlist,
        unknown,
    };
};

template <typename _T, typename _Entry, _Entry _T::*__field, typename _U = _T, int __type = _Entry::type>
struct list {
    static_assert(__type >= 0 && __type < list_type::unknown, "unknown list type.");
};

/* list */
struct list_entry {
    static constexpr int type = list_type::list;
    void *_next;
    void **_prev;
};

template <typename _T, typename _Entry, _Entry _T::*__field, typename _U>
struct list <_T, _Entry, __field, _U, list_type::list> {
public:
    typedef _T type;
    typedef _U value_type;
    typedef _Entry entry_type;

private:
    type *_first;

public:
    class iterator {
    private:
        type *_ptr;
    public:
        iterator(type *ptr = nullptr) noexcept : _ptr(ptr) {}
        iterator(const iterator &x) noexcept : _ptr(x._ptr) {}

        iterator &operator=(const iterator &x) noexcept {
            _ptr = x._ptr;
            return *this;
        }

        value_type& operator*() noexcept {
            return *__LIST_VALUE__(_ptr);
        }

        value_type* operator->() noexcept {
            return __LIST_VALUE__(_ptr);
        }

        value_type* pointer() noexcept {
            return __LIST_VALUE__(_ptr);
        }

        iterator& operator++() noexcept {
            _ptr = __LIST_OBJECT__(__LIST_ENTRY__(_ptr)._next);
            return *this;
        }

        iterator operator++(int)noexcept {
            iterator tmp(_ptr);
            _ptr = __LIST_OBJECT__(__LIST_ENTRY__(_ptr)._next);
            return tmp;
        }

        bool operator==(const iterator &x) const noexcept {
            return _ptr == x._ptr;
        }

        bool operator!=(const iterator &x) const noexcept {
            return _ptr != x._ptr;
        }
    };
public:
    list() noexcept : _first(nullptr) {}
    list(const list &x) = delete;
    list(list &&x) noexcept : _first() {
        swap(x);
    }

    void init() noexcept {
        _first = nullptr;
    }

    void swap(list &x) {
        std::swap(_first, x._first);
        if (_first) {
            _first->_prev = &_first;
        }
        if (x._first) {
            x._first->_prev = &x._first;
        }
    }

    list &operator=(const list &x) = delete;

    list &operator=(list &&x) noexcept {
        swap(x);
        return *this;
    }

    static value_type *insert_back(type *listelm, type *elm) noexcept {
        register entry_type &listed = __LIST_ENTRY__(listelm);
        register entry_type &entry = __LIST_ENTRY__(elm);

        if ((entry._next = listed._next) != nullptr) {
            __LIST_ENTRY__(listed._next)._prev = &entry._next;
        }
        listed._next = elm;
        entry._prev = &listed._next;
        return __LIST_VALUE__(elm);
    }

    static value_type *insert_front(type *listelm, type *elm) noexcept {
        register entry_type &listed = __LIST_ENTRY__(listelm);
        register entry_type &entry = __LIST_ENTRY__(elm);

        entry._prev = listed._prev;
        entry._next = listelm;
        *listed._prev = elm;
        listed._prev = &entry._next;

        return __LIST_VALUE__(elm);
    }

    static value_type *remove(type *elm) noexcept {
        register entry_type &entry = __LIST_ENTRY__(elm);

        if (entry._next != nullptr) {
            __LIST_ENTRY__(entry._next)._prev = entry._prev;
        }
        *entry._prev = entry._next;
        return __LIST_VALUE__(elm);
    }

    static value_type *next(type *elm) noexcept {
        void *p = __LIST_ENTRY__(elm)._next;
        return p ? __LIST_VALUE__(__LIST_OBJECT__(p)) : nullptr;
    }

    value_type *first() noexcept {
        return _first ? __LIST_VALUE__(_first) : nullptr;
    }

    value_type *front() noexcept {
        return _first ? __LIST_VALUE__(_first) : nullptr;
    }

    bool empty() noexcept {
        return !_first;
    }

    value_type *push_front(type *elm) noexcept {
        register entry_type &entry = __LIST_ENTRY__(elm);
        if ((entry._next = _first) != nullptr) {
            __LIST_ENTRY__(_first)._prev = &entry._next;
        }
        _first = elm;
        entry._prev = reinterpret_cast<void**>(&_first);
        return __LIST_VALUE__(elm);
    }

    value_type *pop_front() noexcept {
        if (!_first) {
            return nullptr;
        }
        return remove(_first);
    }

    iterator begin() noexcept {
        return iterator(_first);
    }

    const iterator begin() const noexcept {
        return iterator(_first);
    }

    iterator end() noexcept {
        return iterator(nullptr);
    }

    const iterator end() const noexcept {
        return iterator(nullptr);
    }
};

/* clist */
struct clist_entry {
    static constexpr int type = list_type::clist;

    clist_entry() noexcept {}
    clist_entry(clist_entry *next, clist_entry *prev) noexcept : _next(next), _prev(prev) {}
    clist_entry(clist_entry *next) noexcept : _next(next) {}
    clist_entry(const clist_entry &x) noexcept : _next(x._next), _prev(x._prev) {}

    clist_entry *_next;
    clist_entry *_prev;

    void remove() noexcept {
        _next->_prev = _prev;
        _prev->_next = _next;
    }

    void insert_back(clist_entry *listed) noexcept {
        _next = listed->_next;
        _prev = listed;
        listed->_next->_prev = this;
        listed->_next = this;
    }

    void insert_front(clist_entry *listed) noexcept {
        _next = listed;
        _prev = listed->_prev;
        listed->_prev->_next = this;
        listed->_prev = this;
    }
};

#define __CLIST_OBJECT__(entry) containerof_member(entry, __field)
template <typename _T, typename _Entry, _Entry _T::*__field, typename _U>
struct list <_T, _Entry, __field, _U, list_type::clist>: protected clist_entry {
public:
    typedef _T type;
    typedef _U value_type;
    typedef _Entry entry_type;

    class iterator {
    private:
        entry_type *_entry;
    public:
        iterator(entry_type *entry) noexcept : _entry(entry) {}
        iterator(const iterator &x) noexcept : _entry(x._entry) {}

        iterator &operator=(const iterator &x) noexcept {
            _entry = x._entry;
            return *this;
        }

        value_type& operator*() noexcept {
            return *__LIST_VALUE__(__CLIST_OBJECT__(_entry));
        }

        value_type* operator->() noexcept {
            return __LIST_VALUE__(__CLIST_OBJECT__(_entry));
        }

        value_type* pointer() noexcept {
            return __LIST_VALUE__(__CLIST_OBJECT__(_entry));
        }

        iterator& operator++() noexcept {
            _entry = _entry->_next;
            return *this;
        }

        iterator operator++(int)noexcept {
            iterator tmp(_entry);
            _entry = _entry->_next;
            return tmp;
        }

        iterator& operator--() noexcept {
            _entry = _entry->_prev;
            return *this;
        }

        iterator operator--(int)noexcept {
            iterator tmp(_entry);
            _entry = _entry->_prev;
            return tmp;
        }

        bool operator==(const iterator &x) const noexcept {
            return _entry == x._entry;
        }

        bool operator!=(const iterator &x) const noexcept {
            return _entry != x._entry;
        }
    };


    class reverse_iterator {
    private:
        entry_type *_entry;
    public:
        reverse_iterator(entry_type *entry) noexcept : _entry(entry) {}
        reverse_iterator(const iterator &x) noexcept : _entry(x._entry) {}

        reverse_iterator &operator=(const reverse_iterator &x) noexcept {
            _entry = x._entry;
            return *this;
        }

        value_type& operator*() noexcept {
            return *__LIST_VALUE__(__CLIST_OBJECT__(_entry));
        }

        value_type* operator->() noexcept {
            return __LIST_VALUE__(__CLIST_OBJECT__(_entry));
        }

        value_type* pointer() noexcept {
            return __LIST_VALUE__(__CLIST_OBJECT__(_entry));
        }

        reverse_iterator& operator++() noexcept {
            _entry = _entry->_prev;
            return *this;
        }

        reverse_iterator operator++(int)noexcept {
            iterator tmp(_entry);
            _entry = _entry->_prev;
            return tmp;
        }

        reverse_iterator& operator--() noexcept {
            _entry = _entry->_next;
            return *this;
        }

        reverse_iterator operator--(int)noexcept {
            iterator tmp(_entry);
            _entry = _entry->_next;
            return tmp;
        }

        bool operator==(const reverse_iterator &x) const noexcept {
            return _entry == x._entry;
        }

        bool operator!=(const reverse_iterator &x) const noexcept {
            return _entry != x._entry;
        }
    };

public:
    list() noexcept : clist_entry(this, this) {}
    list(const list &x) = delete;
    list(list &&x) noexcept : clist_entry(this, this) {
        swap(x);
    }

    list &operator=(const list &x) = delete;
    list &operator=(list &&x) noexcept {
        swap(x);
        return *this;
    }

    void init() noexcept {
        _next = this;
        _prev = this;
    }

    void swap(list &x) noexcept {
        std::swap(_next, x._next);
        std::swap(_prev, x._prev);

        _next->_prev = this;
        _prev->_next = this;
        x._next->_prev = std::addressof<list>(x);
        x._prev->_next = std::addressof<list>(x);
    }

    static value_type *insert_back(type *listelm, type *elm) noexcept {
        register entry_type *listed = &(listelm->*__field);
        register entry_type *entry = &(elm->*__field);
        entry->insert_back(listed);
        return __LIST_VALUE__(elm);
    }

    static value_type *insert_front(type *listelm, type *elm) noexcept {
        register entry_type *listed = &(listelm->*__field);
        register entry_type *entry = &(elm->*__field);
        entry->insert_front(listed);
        return __LIST_VALUE__(elm);
    }

    static value_type *remove(type *elm) noexcept {
        register entry_type *entry = &(elm->*__field);
        entry->remove();
        return __LIST_VALUE__(elm);
    }

    value_type *next(type *elm) noexcept {
        register entry_type *entry = &(elm->*__field);
        entry = entry->next;
        return entry == this ? nullptr : __LIST_VALUE__(__CLIST_OBJECT__(entry));
    }

    value_type *prev(type *elm) noexcept {
        register entry_type *entry = &(elm->*__field);
        entry = entry->prev;
        return entry == this ? nullptr : __LIST_VALUE__(__CLIST_OBJECT__(entry));
    }

    value_type *push_front(type *elm) noexcept {
        register entry_type *entry = &(elm->*__field);
        entry->insert_back(this);
        return __LIST_VALUE__(elm);
    }

    value_type *push_back(type *elm) noexcept {
        register entry_type *entry = &(elm->*__field);
        entry->insert_front(this);
        return __LIST_VALUE__(elm);
    }

    value_type *pop_front() noexcept {
        register entry_type *entry = _next;
        if (entry == this) {
            return nullptr;
        }
        else {
            entry->remove();
            return __LIST_VALUE__(__CLIST_OBJECT__(entry));
        }
    }

    value_type *pop_back() noexcept {
        register entry_type *entry = _prev;
        if (entry == this) {
            return nullptr;
        }
        else {
            entry->remove();
            return __LIST_VALUE__(__CLIST_OBJECT__(entry));
        }
    }

    value_type *first() noexcept {
        return _next == this ? nullptr : __LIST_VALUE__(__CLIST_OBJECT__(_next));
    }

    value_type *last() noexcept {
        return _prev == this ? nullptr : __LIST_VALUE__(__CLIST_OBJECT__(_prev));
    }

    value_type *front() noexcept {
        return _next == this ? nullptr : __LIST_VALUE__(__CLIST_OBJECT__(_next));
    }

    value_type *back() noexcept {
        return _prev == this ? nullptr : __LIST_VALUE__(__CLIST_OBJECT__(_prev));
    }

    bool empty() noexcept {
        return _prev == this;
    }

    iterator begin() noexcept {
        return iterator(_next);
    }

    const iterator cbegin() const noexcept {
        return iterator(_next);
    }

    iterator end() noexcept {
        return iterator(this);
    }

    const iterator cend() const noexcept {
        return iterator(this);
    }

    reverse_iterator rbegin() noexcept {
        return reverse_iterator(_prev);
    }

    const reverse_iterator crbegin() const noexcept {
        return reverse_iterator(_prev);
    }

    reverse_iterator rend() noexcept {
        return reverse_iterator(this);
    }

    const reverse_iterator crend() const noexcept {
        return reverse_iterator(this);
    }
};

/* slist_entry */
struct slist_entry {
    static constexpr int type = list_type::slist;

    void *_next;
};

template <typename _T, typename _Entry, _Entry _T::*__field, typename _U>
struct list <_T, _Entry, __field, _U, list_type::slist> {
public:
    typedef _T type;
    typedef _U value_type;
    typedef _Entry entry_type;

private:
    type *_first;

public:
    class iterator {
    private:
        type *_ptr;
    public:
        iterator(type *ptr) noexcept : _ptr(ptr) {}
        iterator(const iterator &x) noexcept : _ptr(x._ptr) {}

        iterator &operator=(const iterator &x) noexcept {
            _ptr = x._ptr;
            return *this;
        }

        value_type& operator*() noexcept {
            return *__LIST_VALUE__(_ptr);
        }

        value_type* operator->() noexcept {
            return __LIST_VALUE__(_ptr);
        }

        value_type* pointer() noexcept {
            return __LIST_VALUE__(_ptr);
        }

        iterator& operator++() noexcept {
            _ptr = __LIST_OBJECT__(__LIST_ENTRY__(_ptr)._next);
            return *this;
        }

        iterator operator++(int)noexcept {
            iterator tmp(_ptr);
            _ptr = __LIST_OBJECT__(__LIST_ENTRY__(_ptr)._next);
            return tmp;
        }

        bool operator==(const iterator &x) const noexcept {
            return _ptr == x._ptr;
        }

        bool operator!=(const iterator &x) const noexcept {
            return _ptr != x._ptr;
        }
    };

public:
    list() noexcept : _first() {}
    list(const list &x) noexcept = delete;
    list(list &&x) noexcept : _first() {
        swap(x);
    }

    list &operator=(const list &x) = delete;

    list &operator=(list &&x) noexcept {
        std::swap(x);
        return *this;
    }

    void init() noexcept {
        _first = nullptr;
    }

    void swap(list &x) {
        std::swap(_first, x._first);
    }

    static value_type *insert_back(type *listelm, type *elm) noexcept {
        register entry_type &listed = __LIST_ENTRY__(listelm);
        register entry_type &entry = __LIST_ENTRY__(elm);

        entry._next = listed._next;
        listed._next = elm;
        return __LIST_VALUE__(elm);
    }

    bool empty() noexcept {
        return !_first;
    }

    value_type *first() noexcept {
        return _first ? __LIST_VALUE__(_first) : nullptr;
    }

    value_type *front() noexcept {
        return _first ? __LIST_VALUE__(_first) : nullptr;
    }

    static value_type *next(type *elm) noexcept {
        void *p = __LIST_ENTRY__(elm)._next;
        return p ? __LIST_VALUE__(__LIST_OBJECT__(p)) : nullptr;
    }

    value_type *push_front(type *elm) noexcept {
        register entry_type &entry = __LIST_ENTRY__(elm);

        entry._next = _first;
        _first = elm;
        return __LIST_VALUE__(elm);
    }

    value_type *pop_front() noexcept {
        type *elm = _first;
        if (elm) {
            _first = __LIST_OBJECT__(__LIST_ENTRY__(elm)._next);
            return __LIST_VALUE__(elm);
        }
        return nullptr;
    }

    value_type *remove(type *elm, type *prev) noexcept {
        if (!prev) {
            return pop_front();
        }
        else {
            register entry_type &entry = __LIST_ENTRY__(prev);
            entry._next = __LIST_ENTRY__(entry._next)._next;
            return __LIST_VALUE__(elm);
        }
    }

    value_type *remove(type *elm) noexcept {
        if (_first == elm) {
            return pop_front();
        }
        else {
            type *curelm = _first;
            register entry_type *entry;
            while (curelm) {
                entry = &__LIST_ENTRY__(curelm);
                if (entry->_next == elm) {
                    break;
                }
                curelm = __LIST_OBJECT__(entry->_next);
            }
            if (curelm) {
                entry->_next = __LIST_ENTRY__(entry->_next)._next;
                return __LIST_VALUE__(elm);
            }
            return nullptr;
        }
    }

    iterator begin() noexcept {
        return iterator(_first);
    }

    const iterator begin() const noexcept {
        return iterator(_first);
    }

    iterator end() noexcept {
        return iterator(nullptr);
    }

    const iterator end() const noexcept {
        return iterator(nullptr);
    }
};

/* stlist */
struct stlist_entry {
    static const int type = list_type::stlist;

    void *_next;
};

template <typename _T, typename _Entry, _Entry _T::*__field, typename _U>
struct list <_T, _Entry, __field, _U, list_type::stlist> {
public:
    typedef _T type;
    typedef _U value_type;
    typedef _Entry entry_type;

private:
    type *_first;
    type **_last;
public:
    class iterator {
    private:
        type *_ptr;
    public:
        iterator(type *ptr) noexcept : _ptr(ptr) {}
        iterator(const iterator &x) noexcept : _ptr(x._ptr) {}

        iterator &operator=(const iterator &x) noexcept {
            _ptr = x._ptr;
            return *this;
        }

        value_type& operator*() noexcept {
            return *__LIST_VALUE__(_ptr);
        }

        value_type* operator->() noexcept {
            return __LIST_VALUE__(_ptr);
        }

        value_type* pointer() noexcept {
            return __LIST_VALUE__(_ptr);
        }

        iterator& operator++() noexcept {
            _ptr = __LIST_OBJECT__(__LIST_ENTRY__(_ptr)._next);
            return *this;
        }

        iterator operator++(int)noexcept {
            iterator tmp(_ptr);
            _ptr = __LIST_OBJECT__(__LIST_ENTRY__(_ptr)._next);
            return tmp;
        }

        bool operator==(const iterator &x) const noexcept {
            return _ptr == x._ptr;
        }

        bool operator!=(const iterator &x) const noexcept {
            return _ptr != x._ptr;
        }
    };

public:
    list() noexcept : _first(), _last(&_first) {}
    list(const list &) = delete;
    list(list &&x) noexcept : _first(), _last(&_first) {
        swap(x);
    }
    list &operator=(const list &) = delete;

    list &operator=(list &&x) noexcept {
        swap(x);
        return *this;
    }
    void swap(list &x) noexcept {
        std::swap(_first, x._first);
        std::swap(_last, x._last);
        if (!_first) {
            _last = &_first;
        }
        if (!x._first) {
            x._last = &x._first;
        }
    }

    void init() noexcept {
        _first = nullptr;
        _last = &_first;
    }

    bool empty() noexcept {
        return !_first;
    }

    value_type *first() noexcept {
        return _first ? __LIST_VALUE__(_first) : nullptr;
    }

    value_type *front() noexcept {
        return _first ? __LIST_VALUE__(_first) : nullptr;
    }

    static value_type *next(type *elm) noexcept {
        void *p = __LIST_ENTRY__(elm)._next;
        return p ? __LIST_VALUE__(__LIST_OBJECT__(p)) : nullptr;
    }

    value_type *push_front(type *elm) noexcept {
        register entry_type &entry = __LIST_ENTRY__(elm);

        if ((entry._next = _first) == nullptr) {
            _last = reinterpret_cast<type**>(&entry._next);
        }
        _first = elm;
        return __LIST_VALUE__(elm);
    }

    value_type *push_back(type *elm) noexcept {
        register entry_type &entry = __LIST_ENTRY__(elm);

        entry._next = nullptr;
        *_last = elm;
        _last = reinterpret_cast<type**>(&entry._next);

        return __LIST_VALUE__(elm);
    }

    value_type* pop_front() noexcept {
        type* elm = _first;
        if (elm) {
            if ((_first = __LIST_OBJECT__(__LIST_ENTRY__(elm)._next)) == nullptr) {
                _last = &_first;
            }
            return __LIST_VALUE__(elm);
        }
        return nullptr;
    }

    value_type *insert_back(type *listelm, type *elm) noexcept {
        register entry_type &listed = __LIST_ENTRY__(listelm);
        register entry_type &entry = __LIST_ENTRY__(elm);

        if ((entry._next = listed._next) == nullptr) {
            _last = &entry._next;
        }
        listed._next = elm;
        return __LIST_VALUE__(elm);
    }

    value_type *remove(type *elm, type *prev) noexcept {
        if (!prev) {
            return pop_front(elm);
        }
        else {
            register entry_type &entry = __LIST_ENTRY__(prev);
            if (ISnullptr(entry._next = __LIST_ENTRY__(entry._next)._next)) {
                _last = &entry._next;
            }
            return __LIST_VALUE__(elm);
        }
    }

    value_type *remove(type *elm) noexcept {
        if (_first == elm) {
            return pop_front();
        }
        else {
            type *curelm = _first;
            register entry_type *entry;
            while (curelm) {
                entry = &__LIST_ENTRY__(curelm);
                if (entry->_next == elm) {
                    break;
                }
                curelm = __LIST_OBJECT__(entry->_next);
            }
            if (curelm) {
                if (!(entry->_next = __LIST_ENTRY__(entry->_next)._next)) {
                    _last = reinterpret_cast<type**>(&entry->_next);
                }
                return __LIST_VALUE__(elm);
            }
            return nullptr;
        }
    }

    iterator begin() noexcept {
        return iterator(_first);
    }

    const iterator begin() const noexcept {
        return iterator(_first);
    }

    iterator end() noexcept {
        return iterator(nullptr);
    }

    const iterator end() const noexcept {
        return iterator(nullptr);
    }
};

#define gv_list(_T, entry, ...) list<                     \
    typename member_of<decltype(&_T::entry)>::class_type, \
    typename member_of<decltype(&_T::entry)>::type,       \
    &_T::entry, ##__VA_ARGS__>

GV_NS_END

#endif
