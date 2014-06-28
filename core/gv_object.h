#ifndef __GV_OBJECT_H__
#define __GV_OBJECT_H__

#include <stack>

#include "gv_platform.h"
#include "gv_list.h"

GV_NS_BEGIN

class Object {
    template <typename> friend class object;
    template <typename> friend class ptr;
    template <typename, int> friend class ptr_list;
    template <typename, typename...> friend class singleton;
    GV_RESTRAIN_NEW();
protected:
    Object() noexcept : _ref(1) {
        if (--_constructRef < 0) {
            constructFailed();
        }
    }
    virtual ~Object() noexcept {
        if (--_destroyRef < 0){
            destroyFailed();
        }
    }
    virtual bool init() { return true;  }
private:
    struct singletons {
        std::stack<Object*> _stack;
        ~singletons();
    };
    mutable size_t _ref;

    static singletons _singletons;
    static void constructFailed();
    static void destroyFailed();

    static int _constructRef;
    static int _destroyRef;
public:
    size_t ref() const {
        return _ref;
    }

    ptr<Object> self() noexcept;
};

template <typename _T>
struct is_object {
    typedef typename std::remove_cv<_T> type;
    static constexpr bool value = std::is_same<Object, type>::value || std::is_base_of<Object, type>::value;
};
template <typename _T>
class ptr {
    friend class Object;
    template <typename> friend class object;
    template <typename> friend class ptr;
    template <typename, int> friend class ptr_list;
    template <typename _T1, typename _T2> friend ptr<_T1> ptr_cast(const ptr<_T2>&);
    template <typename _T1, typename _T2> friend bool operator==(const ptr<_T1>&, const ptr<_T2>&);
    template <typename _T1> friend bool operator==(const ptr<_T1>&, const void*);
    template <typename _T1> friend bool operator==(const void*, const ptr<_T1>&);
public:
    typedef _T type;

private:
    static void *operator new(std::size_t size) {
        return nullptr;
    }
    static void *operator new (std::size_t size, void *ptr) {
        return nullptr;
    }
    static void *operator new[](std::size_t size) {
        return nullptr;
    }
    static void *operator new[](std::size_t size, void *ptr) {
        return nullptr;
    }

    template<typename _Tx = type>
    static typename std::enable_if<
            is_object<_Tx>::value, 
            void
        >::type 
    retain_ptr(_Tx *x) noexcept {
        if (x) {
            ++x->_ref;
        }
    }
    template<typename _Tx = type>
    static typename std::enable_if<
            !is_object<_Tx>::value, 
            void
        >::type 
    retain_ptr(_Tx *x) noexcept {
    }
    template<typename _Tx = type>
    static typename std::enable_if<
            is_object<_Tx>::value, 
            void
        >::type 
    release_ptr(_Tx *x) noexcept {
        if (x) {
            if (--x->_ref) {
                ++Object::_destroyRef;
                delete x;
            }
        }
    }
    template<typename _Tx = type>
    static typename std::enable_if<
            !is_object<_Tx>::value, 
            void
        >::type 
    release_ptr(_Tx *x) noexcept {
        if (x) {
            delete x;
        }
    }

    void retain() noexcept {
        retain_ptr<>(_ptr);
    }
    void retain() const noexcept {
        retain_ptr<>(_ptr);
    }
    void release() noexcept {
        release_ptr<>(_ptr);
    }
    void release() const noexcept {
        release_ptr<>(_ptr);
    }

    ptr(type *x) noexcept : _ptr(x) {
        retain_ptr<>(x);
    }
    ptr(const type *x) noexcept : _ptr(x) {
        retain_ptr<>(x);
    }
    type *_ptr;
public:
    ptr() noexcept : _ptr() {}
    ptr(std::nullptr_t) noexcept : _ptr() {}

    template <typename _Tx>
    ptr(const ptr<_Tx> &x) noexcept {
        retain_ptr<>(_ptr = x._ptr);
    }
    ptr(const ptr &x) noexcept {
        retain_ptr<>(_ptr = x._ptr);
    }
    ptr(ptr &&x) noexcept : _ptr() {
        std::swap(_ptr, x._ptr);
    }
    ~ptr() noexcept {
        static_assert(std::is_base_of<Object, _T>::value, "ptr object must derived from Object.");
        release_ptr<>(_ptr);
    }
    template <typename _Tx>
    void assign(const ptr<_Tx> &x) noexcept {
        if (reinterpret_cast<void*>(_ptr) == reinterpret_cast<void*>(x._ptr)) {
            return;
        }
        release_ptr<>(_ptr);
        retain_ptr<>(_ptr = x._ptr);
    }
    template <typename _Tx>
    ptr &operator=(const ptr<_Tx> &x) noexcept {
        assign(x);
        return *this;
    }
    ptr &operator=(const ptr &x) noexcept {
        assign(x);
        return *this;
    }
    ptr &operator=(std::nullptr_t) noexcept {
        release_ptr<>(_ptr);
        _ptr = nullptr;
        return *this;
    }
    ptr &operator=(ptr &&x) noexcept {
        std::swap(_ptr, x._ptr);
        return *this;
    }
    type *operator->() const noexcept {
        return _ptr;
    }
    operator type*() const noexcept {
        return _ptr;
    }
    type &operator *() const noexcept {
        return *_ptr;
    }
    type *get() const noexcept {
        return _ptr;
    }
};

inline ptr<Object> Object::self() noexcept {
    return ptr<Object>(this);
}

template <typename _T1, typename _T2>
inline bool operator==(const ptr<_T1> &lhs, const ptr<_T2> &rhs) {
    return reinterpret_cast<void*>(lhs._ptr) == reinterpret_cast<void*>(rhs._ptr);
}

template <typename _T>
inline bool operator==(const ptr<_T> &lhs, const void *rhs) {
    return reinterpret_cast<void*>(lhs._ptr) == reinterpret_cast<void*>(rhs);
}

template <typename _T>
inline bool operator==(const void *lhs, const ptr<_T> &rhs) {
    return reinterpret_cast<void*>(lhs) == reinterpret_cast<void*>(rhs._ptr);
}

template <typename _T1, typename _T2>
inline ptr<_T1> ptr_cast(const ptr<_T2> &x) {
    return x._ptr ? ptr<_T1>(static_cast<_T1*>(x._ptr)) : nullptr;
}

/**
 * 
 * 
 */
template <typename _T>
class object final : public ptr<_T> {
public:
    typedef ptr<_T> base;
    typedef _T type;

    template <typename ..._Args>
    object(_Args&&...args) noexcept {
        ++Object::_constructRef;
        base::_ptr = new type(std::forward<_Args>(args)...);
    }
};

/**
 * 
 */
template <typename _T>
class owned_ptr final {
public:
    typedef _T type;
    owned_ptr() noexcept : _ptr() {}
    owned_ptr(type *ptr) noexcept : _ptr(ptr) {}
    owned_ptr(const ptr &x) = delete;
    owned_ptr(owned_ptr &&x) noexcept : _ptr() {
        std::swap(_ptr, x._ptr);
    }
    ~owned_ptr() noexcept {
        release_ptr(_ptr);
    }
    owned_ptr &operator=(const owned_ptr &x) = delete;
    owned_ptr &operator=(type *ptr) noexcept {
        release_ptr(_ptr);
        _ptr = ptr;
        return *this;
    }
    owned_ptr &operator=(owned_ptr &&x) noexcept {
        std::swap(_ptr, x._ptr);
        return *this;
    }
    type *operator->() const noexcept {
        return _ptr;
    }
    operator type*() const noexcept {
        return _ptr;
    }
    type &operator *() const noexcept {
        return *_ptr;
    }
    type *get() const noexcept {
        return _ptr;
    }
private:
    static void *operator new(std::size_t size) {
        return nullptr;
    }
    static void *operator new (std::size_t size, void *ptr) {
        return nullptr;
    }
    static void *operator new[](std::size_t size) {
        return nullptr;
    }
    static void *operator new[](std::size_t size, void *ptr) {
        return nullptr;
    }
    static void release_ptr(type *ptr) noexcept {
        if (ptr) {
            delete ptr;
        }
    }
    type *_ptr;
};

template <typename _T, int __type = _T::entry_type::type>
class ptr_list {};

template <typename _List>
class ptr_list<_List, list_type::list> : private _List {
public:
    typedef _List                       base;
    typedef typename base::type         type;
    typedef typename base::value_type   value_type;
    typedef ptr<value_type>             ptr_type;

    using base::swap;
    using base::operator=;
    using base::first;
    using base::front;
    using base::next;
    using base::empty;
    using base::iterator;
    using base::begin;
    using base::end;

    ptr_list() noexcept : base() {}
    ptr_list(const ptr_list &) = delete;
    ptr_list(ptr_list &&x) noexcept : base(std::move(x)) {}

    ~ptr_list() noexcept {
        clear();
    }

    void clear() noexcept {
        while (!empty()) {
            pop_front();
        }
    }

    static value_type *insert_back(type *listelm, const ptr_type &elm) noexcept {
        elm.retain();
        return base::insert_back(listelm, elm);
    }

    static value_type *insert_front(type *listelm, const ptr_type &elm) noexcept {
        elm.retain();
        return base::insert_front(listelm, elm);
    }

    static ptr_type remove(type *elm) noexcept {
        ptr_type ret;
        ret._ptr = base::remove(elm);
        return ret;
    }

    value_type *push_front(const ptr_type &elm) noexcept {
        elm.retain();
        return base::push_front(elm);
    }

    ptr_type pop_front() noexcept {
        ptr_type ret;
        ret._ptr = base::pop_front();
        return ret;
    }

};

template <typename _List>
class ptr_list<_List, list_type::clist> : private _List {
public:
    typedef _List                       base;
    typedef typename base::type         type;
    typedef typename base::value_type   value_type;
    typedef ptr<value_type>             ptr_type;

    using base::swap;
    using base::operator=;
    using base::first;
    using base::front;
    using base::last;
    using base::back;
    using base::next;
    using base::prev;
    using base::empty;
    using base::iterator;
    using base::reverse_iterator;
    using base::begin;
    using base::end;
    using base::rbegin;
    using base::rend;

    ptr_list() noexcept : base() {}
    ptr_list(const ptr_list &) = delete;
    ptr_list(ptr_list &&x) noexcept : base(std::move(x)) {}

    ~ptr_list() noexcept {
        clear();
    }

    void clear() noexcept {
        while (!empty()) {
            pop_front();
        }
    }

    static value_type *insert_back(type *listelm, const ptr_type &elm) noexcept {
        elm.retain();
        return base::insert_back(listelm, elm);
    }

    static value_type *insert_front(type *listelm, const ptr_type &elm) noexcept {
        elm.retain();
        return base::insert_front(listelm, elm);
    }

    static ptr_type remove(type *elm) noexcept {
        ptr_type ret;
        ret._ptr = base::remove(elm);
        return ret;
    }

    value_type *push_front(const ptr_type &elm) noexcept {
        elm.retain();
        return base::push_front(elm);
    }

    ptr_type pop_front() noexcept {
        ptr_type ret;
        ret._ptr = base::pop_front();
        return ret;
    }

    value_type *push_back(const ptr_type &elm) noexcept {
        elm.retain();
        return base::push_back(elm);
    }

    ptr_type pop_back() noexcept {
        ptr_type ret;
        ret._ptr = base::pop_back();
        return ret;
    }
};

template <typename _List>
class ptr_list<_List, list_type::slist> : private _List {
public:
    typedef _List                       base;
    typedef typename base::type         type;
    typedef typename base::value_type   value_type;
    typedef ptr<value_type>             ptr_type;

    using base::swap;
    using base::operator=;
    using base::first;
    using base::front;
    using base::next;
    using base::empty;
    using base::iterator;
    using base::begin;
    using base::end;

    ptr_list() noexcept : base() {}
    ptr_list(const ptr_list &) = delete;
    ptr_list(ptr_list &&x) noexcept : base(std::move(x)) {}

    ~ptr_list() noexcept {
        clear();
    }

    void clear() noexcept {
        while (!empty()) {
            pop_front();
        }
    }

    value_type *insert_back(type *listelm, const ptr_type &elm) noexcept {
        elm.retain();
        return base::insert_back(listelm, elm);
    }

    ptr_type remove(type *elm, type *prev) noexcept {
        ptr_type ret;
        ret._ptr = base::remove(elm, prev);
        return ret;
    }

    ptr_type remove(type *elm) noexcept {
        ptr_type ret;
        ret._ptr = base::remove(elm);
        return ret;
    }

    value_type *push_front(const ptr_type &elm) noexcept {
        elm.retain();
        return base::push_front(elm);
    }

    ptr_type pop_front() noexcept {
        ptr_type ret;
        ret._ptr = base::pop_front();
        return ret;
    }
};

template <typename _List>
class ptr_list<_List, list_type::stlist> : private _List {
public:
    typedef _List                       base;
    typedef typename base::type         type;
    typedef typename base::value_type   value_type;
    typedef ptr<value_type>             ptr_type;

    using base::swap;
    using base::operator=;
    using base::first;
    using base::front;
    using base::next;
    using base::empty;
    using base::iterator;
    using base::begin;
    using base::end;

    ptr_list() noexcept : base() {}
    ptr_list(const ptr_list &) = delete;
    ptr_list(ptr_list &&x) noexcept : base(std::move(x)) {}

    ~ptr_list() noexcept {
        clear();
    }

    void clear() noexcept {
        while (!empty()) {
            pop_front();
        }
    }

    static value_type *insert_back(type *listelm, const ptr_type &elm) noexcept {
        elm.retain();
        return base::insert_back(listelm, elm);
    }

    ptr_type remove(type *elm, type *prev) noexcept {
        ptr_type ret;
        ret._ptr = base::remove(elm, prev);
        return ret;
    }

    ptr_type remove(type *elm) noexcept {
        ptr_type ret;
        ret._ptr = base::remove(elm);
        return ret;
    }

    value_type *push_front(const ptr_type &elm) noexcept {
        elm.retain();
        return base::push_front(elm);
    }

    ptr_type pop_front() noexcept {
        ptr_type ret;
        ret._ptr = base::pop_front();
        return ret;
    }

    value_type *push_back(const ptr_type &elm) noexcept {
        elm.retain();
        return base::push_back(elm);
    }

};

#define gv_ptr_list(_T, entry, ...) gv::ptr_list<gv_list(_T, entry, ##__VA_ARGS__)>

#define GV_FRIEND_PTR()                      \
    template <typename> friend class object; \
    template <typename> friend class ptr;

GV_NS_END
#endif
