#ifndef __GV_LIST_H__
#define __GV_LIST_H__

#include "gv_platform.h"

GV_NS_BEGIN

#define __GV_LIST_OBJECT__(x)   static_cast<type*>(x)
#define __GV_LIST_ENTRY__(x)    static_cast<entry_type&>(__GV_LIST_OBJECT__(x)->*(__field))
#define __GV_LIST_VALUE__(x)    static_cast<type*>(x)

enum class ListType : unsigned {
    list,
    clist,
    slist,
    stlist,
    unknown,
};

template <
    typename _T,
    typename _U,
    typename _Entry, 
    _Entry _U::*__field,
    ListType __type,
    bool>
struct bsdlist;

#define GV_FRIEND_LIST() \
template <               \
    typename,            \
    typename _Ux,        \
    typename _Entryx,    \
    _Entryx _Ux::*,      \
    GV_NS::ListType,     \
    bool>                \
friend struct GV_NS::bsdlist

/* list */
struct list_entry {
    static constexpr ListType list_type = ListType::list;
    void *_next;
    void **_prev;
};

template <typename _T, typename _U, typename _Entry, _Entry _U::*__field>
struct bsdlist <_T, _U, _Entry, __field, ListType::list, false> {
public:
    static_assert(std::is_same<_U, _T>::value || std::is_base_of<_U, _T>::value,
                  "list _T must same with _U or derived from _U");
    typedef _T          type;
    typedef type*       pointer;
    typedef _Entry      entry_type;

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

        pointer operator*() noexcept {
            return __GV_LIST_VALUE__(_ptr);
        }

        pointer operator->() noexcept {
            return __GV_LIST_VALUE__(_ptr);
        }

        iterator& operator++() noexcept {
            _ptr = __GV_LIST_OBJECT__(__GV_LIST_ENTRY__(_ptr)._next);
            return *this;
        }

        iterator operator++(int)noexcept {
            iterator tmp(_ptr);
            _ptr = __GV_LIST_OBJECT__(__GV_LIST_ENTRY__(_ptr)._next);
            return tmp;
        }

        bool operator==(const iterator &x) const noexcept {
            return _ptr == x._ptr;
        }

        bool operator!=(const iterator &x) const noexcept {
            return _ptr != x._ptr;
        }
    };
    typedef const iterator const_iterator;
public:
    bsdlist() noexcept : _first(nullptr) {}
    bsdlist(const bsdlist &x) = delete;
    bsdlist(bsdlist &&x) noexcept : _first() {
        swap(x);
    }

    void init() noexcept {
        _first = nullptr;
    }

    void swap(bsdlist &x) {
        std::swap(_first, x._first);
        if (_first) {
            __GV_LIST_ENTRY__(_first)._prev = reinterpret_cast<void**>(&_first);
        }
        if (x._first) {
            __GV_LIST_ENTRY__(x._first)._prev = reinterpret_cast<void**>(&x._first);
        }
    }

    bsdlist &operator=(const bsdlist &x) = delete;

    bsdlist &operator=(bsdlist &&x) noexcept {
        swap(x);
        return *this;
    }

    static pointer insert_back(type *listelm, type *elm) noexcept {
        register entry_type &listed = __GV_LIST_ENTRY__(listelm);
        register entry_type &entry = __GV_LIST_ENTRY__(elm);

        if ((entry._next = listed._next) != nullptr) {
            __GV_LIST_ENTRY__(listed._next)._prev = &entry._next;
        }
        listed._next = elm;
        entry._prev = &listed._next;
        return __GV_LIST_VALUE__(elm);
    }

    static pointer insert_front(type *listelm, type *elm) noexcept {
        register entry_type &listed = __GV_LIST_ENTRY__(listelm);
        register entry_type &entry = __GV_LIST_ENTRY__(elm);

        entry._prev = listed._prev;
        entry._next = listelm;
        *listed._prev = elm;
        listed._prev = &entry._next;

        return __GV_LIST_VALUE__(elm);
    }

    static pointer remove(type *elm) noexcept {
        register entry_type &entry = __GV_LIST_ENTRY__(elm);

        if (entry._next != nullptr) {
            __GV_LIST_ENTRY__(entry._next)._prev = entry._prev;
        }
        *entry._prev = entry._next;
        return __GV_LIST_VALUE__(elm);
    }

    static pointer next(type *elm) noexcept {
        void *p = __GV_LIST_ENTRY__(elm)._next;
        return p ? __GV_LIST_VALUE__(__GV_LIST_OBJECT__(p)) : nullptr;
    }

    pointer first() const noexcept {
        return _first ? __GV_LIST_VALUE__(_first) : nullptr;
    }

    pointer front() const noexcept {
        return _first ? __GV_LIST_VALUE__(_first) : nullptr;
    }

    bool empty() const noexcept {
        return !_first;
    }

    pointer push_front(type *elm) noexcept {
        register entry_type &entry = __GV_LIST_ENTRY__(elm);
        if ((entry._next = _first) != nullptr) {
            __GV_LIST_ENTRY__(_first)._prev = &entry._next;
        }
        _first = elm;
        entry._prev = reinterpret_cast<void**>(&_first);
        return __GV_LIST_VALUE__(elm);
    }

    pointer pop_front() noexcept {
        if (!_first) {
            return nullptr;
        }
        return remove(_first);
    }

    iterator begin() noexcept {
        return iterator(_first);
    }

    const_iterator begin() const noexcept {
        return iterator(_first);
    }

    const_iterator cbegin() const noexcept {
        return iterator(_first);
    }

    iterator end() noexcept {
        return iterator(nullptr);
    }

    const_iterator end() const noexcept {
        return iterator(nullptr);
    }

    const_iterator cend() const noexcept {
        return iterator(nullptr);
    }
private:
    type *_first;
};

/* clist */
struct clist_entry {
    static constexpr ListType list_type = ListType::clist;

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

#define __CLIST_OBJECT__(entry) static_cast<pointer>(containerof_member(entry, __field))
template <typename _T, typename _U, typename _Entry, _Entry _U::*__field>
struct bsdlist <_T, _U, _Entry, __field, ListType::clist, false>: protected clist_entry {
public:
    static_assert(std::is_same<_U, _T>::value || std::is_base_of<_U, _T>::value,
                  "list _T must same with _U or derived from _U");

    typedef _T          type;
    typedef type*       pointer;
    typedef _Entry      entry_type;

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

        pointer operator*() noexcept {
            return __GV_LIST_VALUE__(__CLIST_OBJECT__(_entry));
        }

        pointer operator->() noexcept {
            return __GV_LIST_VALUE__(__CLIST_OBJECT__(_entry));
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

        pointer operator*() noexcept {
            return __GV_LIST_VALUE__(__CLIST_OBJECT__(_entry));
        }

        pointer operator->() noexcept {
            return __GV_LIST_VALUE__(__CLIST_OBJECT__(_entry));
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

    typedef const iterator const_iterator;
    typedef const reverse_iterator const_reverse_iterator;

public:
    bsdlist() noexcept {
        init();
    }
    bsdlist(const bsdlist &x) = delete;
    bsdlist(bsdlist &&x) noexcept {
        if (x.empty()){
            init();
        }
        else {
            _next = x._next; 
            _prev = x._prev;
            _next->_prev = this;
            _prev->_next = this;
            x.init();
        }
    }

    bsdlist &operator=(const bsdlist &x) = delete;
    bsdlist &operator=(bsdlist &&x) noexcept {
        swap(x);
        return *this;
    }

    void init() noexcept {
        _next = this;
        _prev = this;
    }

    void swap(bsdlist &x) noexcept {
        std::swap(_next, x._next);
        std::swap(_prev, x._prev);

        _next->_prev = this;
        _prev->_next = this;
        x._next->_prev = std::addressof<bsdlist>(x);
        x._prev->_next = std::addressof<bsdlist>(x);
    }

    static pointer insert_back(type *listelm, type *elm) noexcept {
        register entry_type *listed = &(listelm->*__field);
        register entry_type *entry = &(elm->*__field);
        entry->insert_back(listed);
        return __GV_LIST_VALUE__(elm);
    }

    static pointer insert_front(type *listelm, type *elm) noexcept {
        register entry_type *listed = &(listelm->*__field);
        register entry_type *entry = &(elm->*__field);
        entry->insert_front(listed);
        return __GV_LIST_VALUE__(elm);
    }

    static pointer remove(type *elm) noexcept {
        register entry_type *entry = &(elm->*__field);
        entry->remove();
        return __GV_LIST_VALUE__(elm);
    }

    pointer next(type *elm) noexcept {
        register entry_type *entry = &(elm->*__field);
        entry = entry->_next;
        return entry == this ? nullptr : __GV_LIST_VALUE__(__CLIST_OBJECT__(entry));
    }

    pointer prev(type *elm) noexcept {
        register entry_type *entry = &(elm->*__field);
        entry = entry->_prev;
        return entry == this ? nullptr : __GV_LIST_VALUE__(__CLIST_OBJECT__(entry));
    }

    pointer push_front(type *elm) noexcept {
        register entry_type *entry = &(elm->*__field);
        entry->insert_back(this);
        return __GV_LIST_VALUE__(elm);
    }

    pointer push_back(type *elm) noexcept {
        register entry_type *entry = &(elm->*__field);
        entry->insert_front(this);
        return __GV_LIST_VALUE__(elm);
    }

    pointer pop_front() noexcept {
        register entry_type *entry = _next;
        if (entry == this) {
            return nullptr;
        }
        else {
            entry->remove();
            return __GV_LIST_VALUE__(__CLIST_OBJECT__(entry));
        }
    }

    pointer pop_back() noexcept {
        register entry_type *entry = _prev;
        if (entry == this) {
            return nullptr;
        }
        else {
            entry->remove();
            return __GV_LIST_VALUE__(__CLIST_OBJECT__(entry));
        }
    }

    pointer first() const noexcept {
        return _next == this ? nullptr : __GV_LIST_VALUE__(__CLIST_OBJECT__(_next));
    }

    pointer last() const noexcept {
        return _prev == this ? nullptr : __GV_LIST_VALUE__(__CLIST_OBJECT__(_prev));
    }

    pointer front() const noexcept {
        return _next == this ? nullptr : __GV_LIST_VALUE__(__CLIST_OBJECT__(_next));
    }

    pointer back() const noexcept {
        return _prev == this ? nullptr : __GV_LIST_VALUE__(__CLIST_OBJECT__(_prev));
    }

    bool empty() const noexcept {
        return _prev == this;
    }

    iterator begin() noexcept {
        return iterator(_next);
    }

    const_iterator begin() const noexcept {
        return iterator(_next);
    }

    const_iterator cbegin() const noexcept {
        return iterator(_next);
    }

    iterator end() noexcept {
        return iterator(this);
    }

    const_iterator end() const noexcept {
        return iterator(this);
    }

    const_iterator cend() const noexcept {
        return iterator(this);
    }

    reverse_iterator rbegin() noexcept {
        return reverse_iterator(_prev);
    }

    const_reverse_iterator rbegin() const noexcept {
        return reverse_iterator(_prev);
    }

    const_reverse_iterator crbegin() const noexcept {
        return reverse_iterator(_prev);
    }

    reverse_iterator rend() noexcept {
        return reverse_iterator(this);
    }

    const_reverse_iterator rend() const noexcept {
        return reverse_iterator(this);
    }

    const_reverse_iterator crend() const noexcept {
        return reverse_iterator(this);
    }
};

/* slist_entry */
struct slist_entry {
    static constexpr ListType list_type = ListType::slist;

    void *_next;
};

template <typename _T, typename _U, typename _Entry, _Entry _U::*__field>
struct bsdlist <_T, _U, _Entry, __field, ListType::slist, false> {
public:
    static_assert(std::is_same<_U, _T>::value || std::is_base_of<_U, _T>::value,
                  "list _T must same with _U or derived from _U");

    typedef _T          type;
    typedef type*       pointer;
    typedef _Entry      entry_type;

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

        pointer operator*() noexcept {
            return __GV_LIST_VALUE__(_ptr);
        }

        pointer operator->() noexcept {
            return __GV_LIST_VALUE__(_ptr);
        }

        iterator& operator++() noexcept {
            _ptr = __GV_LIST_OBJECT__(__GV_LIST_ENTRY__(_ptr)._next);
            return *this;
        }

        iterator operator++(int)noexcept {
            iterator tmp(_ptr);
            _ptr = __GV_LIST_OBJECT__(__GV_LIST_ENTRY__(_ptr)._next);
            return tmp;
        }

        bool operator==(const iterator &x) const noexcept {
            return _ptr == x._ptr;
        }

        bool operator!=(const iterator &x) const noexcept {
            return _ptr != x._ptr;
        }
    };
    typedef const iterator const_iterator;
public:
    bsdlist() noexcept : _first() {}
    bsdlist(const bsdlist &x) noexcept = delete;
    bsdlist(bsdlist &&x) noexcept : _first() {
        swap(x);
    }

    bsdlist &operator=(const bsdlist &x) = delete;

    bsdlist &operator=(bsdlist &&x) noexcept {
        std::swap(x);
        return *this;
    }

    void init() noexcept {
        _first = nullptr;
    }

    void swap(bsdlist &x) {
        std::swap(_first, x._first);
    }

    static pointer insert_back(type *listelm, type *elm) noexcept {
        register entry_type &listed = __GV_LIST_ENTRY__(listelm);
        register entry_type &entry = __GV_LIST_ENTRY__(elm);

        entry._next = listed._next;
        listed._next = elm;
        return __GV_LIST_VALUE__(elm);
    }

    bool empty() const noexcept {
        return !_first;
    }

    pointer first() const noexcept {
        return _first ? __GV_LIST_VALUE__(_first) : nullptr;
    }

    pointer front() const noexcept {
        return _first ? __GV_LIST_VALUE__(_first) : nullptr;
    }

    static pointer next(type *elm) noexcept {
        void *p = __GV_LIST_ENTRY__(elm)._next;
        return p ? __GV_LIST_VALUE__(__GV_LIST_OBJECT__(p)) : nullptr;
    }

    pointer push_front(type *elm) noexcept {
        register entry_type &entry = __GV_LIST_ENTRY__(elm);

        entry._next = _first;
        _first = elm;
        return __GV_LIST_VALUE__(elm);
    }

    pointer pop_front() noexcept {
        type *elm = _first;
        if (elm) {
            _first = __GV_LIST_OBJECT__(__GV_LIST_ENTRY__(elm)._next);
            return __GV_LIST_VALUE__(elm);
        }
        return nullptr;
    }

    pointer remove(type *elm, type *prev) noexcept {
        if (!prev) {
            return pop_front();
        }
        else {
            register entry_type &entry = __GV_LIST_ENTRY__(prev);
            entry._next = __GV_LIST_ENTRY__(entry._next)._next;
            return __GV_LIST_VALUE__(elm);
        }
    }

    pointer remove(type *elm) noexcept {
        if (_first == elm) {
            return pop_front();
        }
        else {
            type *curelm = _first;
            register entry_type *entry;
            while (curelm) {
                entry = &__GV_LIST_ENTRY__(curelm);
                if (entry->_next == elm) {
                    break;
                }
                curelm = __GV_LIST_OBJECT__(entry->_next);
            }
            if (curelm) {
                entry->_next = __GV_LIST_ENTRY__(entry->_next)._next;
                return __GV_LIST_VALUE__(elm);
            }
            return nullptr;
        }
    }

    iterator begin() noexcept {
        return iterator(_first);
    }

    const_iterator begin() const noexcept {
        return iterator(_first);
    }

    const_iterator cbegin() const noexcept {
        return iterator(_first);
    }

    iterator end() noexcept {
        return iterator(nullptr);
    }

    const_iterator end() const noexcept {
        return iterator(nullptr);
    }

    const_iterator cend() const noexcept {
        return iterator(nullptr);
    }

private:
    type *_first;
};

/* stlist */
struct stlist_entry {
    static constexpr ListType list_type = ListType::stlist;

    void *_next;
};

template <typename _T, typename _U, typename _Entry, _Entry _U::*__field>
struct bsdlist <_T, _U, _Entry, __field, ListType::stlist, false> {
public:
    static_assert(std::is_same<_U, _T>::value || std::is_base_of<_U, _T>::value,
                  "list _T must same with _U or derived from _U");

    typedef _T          type;
    typedef type*       pointer;
    typedef _Entry      entry_type;

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

        pointer operator*() noexcept {
            return __GV_LIST_VALUE__(_ptr);
        }

        pointer operator->() noexcept {
            return __GV_LIST_VALUE__(_ptr);
        }

        iterator& operator++() noexcept {
            _ptr = __GV_LIST_OBJECT__(__GV_LIST_ENTRY__(_ptr)._next);
            return *this;
        }

        iterator operator++(int)noexcept {
            iterator tmp(_ptr);
            _ptr = __GV_LIST_OBJECT__(__GV_LIST_ENTRY__(_ptr)._next);
            return tmp;
        }

        bool operator==(const iterator &x) const noexcept {
            return _ptr == x._ptr;
        }

        bool operator!=(const iterator &x) const noexcept {
            return _ptr != x._ptr;
        }
    };
    typedef const iterator const_iterator;
public:
    bsdlist() noexcept : _first(), _last(&_first) {}
    bsdlist(const bsdlist &) = delete;
    bsdlist(bsdlist &&x) noexcept : _first(), _last(&_first) {
        swap(x);
    }
    bsdlist &operator=(const bsdlist &) = delete;

    bsdlist &operator=(bsdlist &&x) noexcept {
        swap(x);
        return *this;
    }
    void swap(bsdlist &x) noexcept {
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

    bool empty() const noexcept {
        return !_first;
    }

    pointer first() const noexcept {
        return _first ? __GV_LIST_VALUE__(_first) : nullptr;
    }

    pointer front() const noexcept {
        return _first ? __GV_LIST_VALUE__(_first) : nullptr;
    }

    static pointer next(type *elm) noexcept {
        void *p = __GV_LIST_ENTRY__(elm)._next;
        return p ? __GV_LIST_VALUE__(__GV_LIST_OBJECT__(p)) : nullptr;
    }

    pointer push_front(type *elm) noexcept {
        register entry_type &entry = __GV_LIST_ENTRY__(elm);

        if ((entry._next = _first) == nullptr) {
            _last = reinterpret_cast<type**>(&entry._next);
        }
        _first = elm;
        return __GV_LIST_VALUE__(elm);
    }

    pointer push_back(type *elm) noexcept {
        register entry_type &entry = __GV_LIST_ENTRY__(elm);

        entry._next = nullptr;
        *_last = elm;
        _last = reinterpret_cast<type**>(&entry._next);

        return __GV_LIST_VALUE__(elm);
    }

    pointer pop_front() noexcept {
        type* elm = _first;
        if (elm) {
            if ((_first = __GV_LIST_OBJECT__(__GV_LIST_ENTRY__(elm)._next)) == nullptr) {
                _last = &_first;
            }
            return __GV_LIST_VALUE__(elm);
        }
        return nullptr;
    }

    pointer insert_back(type *listelm, type *elm) noexcept {
        register entry_type &listed = __GV_LIST_ENTRY__(listelm);
        register entry_type &entry = __GV_LIST_ENTRY__(elm);

        if ((entry._next = listed._next) == nullptr) {
            _last = &entry._next;
        }
        listed._next = elm;
        return __GV_LIST_VALUE__(elm);
    }

    pointer remove(type *elm, type *prev) noexcept {
        if (!prev) {
            return pop_front(elm);
        }
        else {
            register entry_type &entry = __GV_LIST_ENTRY__(prev);
            if (ISnullptr(entry._next = __GV_LIST_ENTRY__(entry._next)._next)) {
                _last = &entry._next;
            }
            return __GV_LIST_VALUE__(elm);
        }
    }

    pointer remove(type *elm) noexcept {
        if (_first == elm) {
            return pop_front();
        }
        else {
            type *curelm = _first;
            register entry_type *entry;
            while (curelm) {
                entry = &__GV_LIST_ENTRY__(curelm);
                if (entry->_next == elm) {
                    break;
                }
                curelm = __GV_LIST_OBJECT__(entry->_next);
            }
            if (curelm) {
                if (!(entry->_next = __GV_LIST_ENTRY__(entry->_next)._next)) {
                    _last = reinterpret_cast<type**>(&entry->_next);
                }
                return __GV_LIST_VALUE__(elm);
            }
            return nullptr;
        }
    }

    iterator begin() noexcept {
        return iterator(_first);
    }

    const_iterator begin() const noexcept {
        return iterator(_first);
    }

    const_iterator cbegin() const noexcept {
        return iterator(_first);
    }

    iterator end() noexcept {
        return iterator(nullptr);
    }

    const_iterator end() const noexcept {
        return iterator(nullptr);
    }

    const_iterator cend() const noexcept {
        return iterator(nullptr);
    }
private:
    type *_first;
    type **_last;
};

GV_NS_END

#endif
