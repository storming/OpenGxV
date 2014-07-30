#ifndef __GV_OBJECT_H__
#define __GV_OBJECT_H__

#include <stack>

#include "gv_platform.h"
#include "gv_list.h"

GV_NS_BEGIN

class Object {
    template <typename> friend class object;
    template <typename> friend class ptr;
    template <typename, typename...> friend class singleton;
    GV_FRIEND_LIST();
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

    static void constructFailed();
    static void destroyFailed();
    template <typename _T, typename ..._Args>
    static _T *constructObject(_Args&&...args) noexcept {
        ++_constructRef;
        return new _T(std::forward<_Args>(args)...);
    }
    static void destroyObject(Object *obj) noexcept {
        ++_destroyRef;
        delete obj;
    }
    template <typename _T>
    static _T *constructSingleton() noexcept {
        _T *obj = constructObject<_T>();
        if (!obj->init()){
            destroyObject(obj);
            return nullptr;
        }
        _singletons._stack.push(obj); 
        return obj;
    }
    mutable size_t _ref;
    static int _constructRef;
    static int _destroyRef;
    static singletons _singletons;
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
    template <typename _T1, typename _T2> friend ptr<_T1> ptr_cast(const ptr<_T2>&);
    template <
        typename _Key,
        typename _Tx,
        typename _U,
        typename _Entry,
        _Entry _U::*__field,
        typename _Compare,
        bool __left_acc,
        bool __right_acc>
    friend class rbmap;
    GV_FRIEND_LIST();
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
    static void retain_ptr(type *x) noexcept {
        if (x) {
            ++x->_ref;
        }
    }
    static void retain_ptr(const type *x) noexcept {
        if (x) {
            ++x->_ref;
        }
    }
    static void release_ptr(type *x) noexcept {
        if (x && !--x->_ref) {
            Object::destroyObject(x);
        }
    }
    static void release_ptr(const type *x) noexcept {
        if (x && !--x->_ref) {
            Object::destroyObject(x);
        }
    }
    void retain() noexcept {
        retain_ptr(_ptr);
    }
    void retain() const noexcept {
        retain_ptr(_ptr);
    }
    void release() noexcept {
        release_ptr(_ptr);
    }
    void release() const noexcept {
        release_ptr(_ptr);
    }
    type *_ptr;
public:
    ptr() noexcept : _ptr() {}
    ptr(std::nullptr_t) noexcept : _ptr() {}

    ptr(type *x) noexcept : _ptr(x) {
        retain_ptr(x);
    }
    template <typename _Tx>
    ptr(const ptr<_Tx> &x) noexcept {
        retain_ptr(_ptr = x._ptr);
    }
    ptr(const ptr &x) noexcept {
        retain_ptr(_ptr = x._ptr);
    }
    ptr(ptr &&x) noexcept : _ptr() {
        std::swap(_ptr, x._ptr);
    }
    ~ptr() noexcept {
        static_assert(std::is_base_of<Object, _T>::value, 
                      "ptr object must derived from Object.");
        release_ptr(_ptr);
    }
    template <typename _Tx>
    void assign(const ptr<_Tx> &x) noexcept {
        if (reinterpret_cast<void*>(_ptr) == reinterpret_cast<void*>(x._ptr)) {
            return;
        }
        release_ptr(_ptr);
        retain_ptr(_ptr = x._ptr);
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
    ptr &operator=(type *x) noexcept {
        release_ptr(_ptr);
        retain_ptr(_ptr = x);
        return *this;
    }
    ptr &operator=(ptr &&x) noexcept {
        std::swap(_ptr, x._ptr);
        return *this;
    }
    type *operator->() const noexcept {
        return _ptr;
    }
    type *get() const noexcept {
        return _ptr;
    }
    operator type*() const noexcept {
        return _ptr;
    }
    type &operator*() const noexcept {
        return *_ptr;
    }
};

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
        base::_ptr = Object::constructObject<type>(std::forward<_Args>(args)...);
    }
};

/**
 * 
 */
template <typename _T>
struct is_ptr : std::false_type {
    typedef _T type;
};

template <typename _T>
struct is_ptr<ptr<_T>> : std::true_type {
    typedef _T type;
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
    owned_ptr(const owned_ptr &x) = delete;
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

template <typename _T, typename _U, typename _Entry, _Entry _U::*__field>
struct bsdlist<_T, _U, _Entry, __field, ListType::list, true> : private bsdlist<typename is_ptr<_T>::type, _U, _Entry, __field, ListType::list, false> {
public:
    typedef bsdlist<
        typename is_ptr<_T>::type, 
        _U, _Entry, __field, 
        ListType::clist, false>     base;
    typedef typename base::type     type;
    typedef _T                      pointer;
    typedef const pointer           const_pointer;

    using base::swap;
    using base::operator=;
    using base::first;
    using base::front;
    using base::next;
    using base::empty;
    using base::iterator;
    using base::begin;
    using base::end;
    using base::cbegin;
    using base::cend;

    bsdlist() noexcept : base() {}
    bsdlist(const bsdlist&) = delete;
    bsdlist(bsdlist &&x) noexcept : base(std::move(x)) {}

    ~bsdlist() noexcept {
        clear();
    }

    void clear() noexcept {
        while (!empty()) {
            pop_front();
        }
    }

    static pointer insert_back(type *listelm, const_pointer &elm) noexcept {
        elm.retain();
        base::insert_back(listelm, elm);
        return elm;
    }

    static pointer insert_front(type *listelm, const_pointer &elm) noexcept {
        elm.retain();
        base::insert_front(listelm, elm);
        return elm;
    }

    static pointer remove(type *elm) noexcept {
        pointer ret;
        ret._ptr = base::remove(elm);
        return ret;
    }

    pointer push_front(const_pointer &elm) noexcept {
        elm.retain();
        base::push_front(elm);
        return elm;
    }

    pointer pop_front() noexcept {
        pointer ret;
        ret._ptr = base::pop_front();
        return ret;
    }
};

template <typename _T, typename _U, typename _Entry, _Entry _U::*__field>
struct bsdlist<_T, _U, _Entry, __field, ListType::clist, true> : private bsdlist<typename is_ptr<_T>::type, _U, _Entry, __field, ListType::clist, false> {
public:
    typedef bsdlist<
        typename is_ptr<_T>::type, 
        _U, _Entry, __field, 
        ListType::clist, false>     base;
    typedef typename base::type     type;
    typedef _T                      pointer;
    typedef const pointer           const_pointer;

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
    using base::cbegin;
    using base::cend;
    using base::rbegin;
    using base::rend;
    using base::crbegin;
    using base::crend;

    bsdlist() noexcept : base() {}
    bsdlist(const bsdlist&) = delete;
    bsdlist(bsdlist &&x) noexcept : base(std::move(x)) {}

    ~bsdlist() noexcept {
        clear();
    }

    void clear() noexcept {
        while (!empty()) {
            pop_front();
        }
    }

    static pointer insert_back(type *listelm, const_pointer &elm) noexcept {
        elm.retain();
        base::insert_back(listelm, elm);
        return elm;
    }

    static pointer insert_front(type *listelm, const_pointer &elm) noexcept {
        elm.retain();
        base::insert_front(listelm, elm);
        return elm;
    }

    static pointer remove(type *elm) noexcept {
        pointer ret;
        ret._ptr = base::remove(elm);
        return ret;
    }

    pointer push_front(const_pointer &elm) noexcept {
        elm.retain();
        base::push_front(elm);
        return elm;
    }

    pointer pop_front() noexcept {
        pointer ret;
        ret._ptr = base::pop_front();
        return ret;
    }

    pointer push_back(const_pointer &elm) noexcept {
        elm.retain();
        base::push_back(elm);
        return elm;
    }

    pointer pop_back() noexcept {
        pointer ret;
        ret._ptr = base::pop_back();
        return ret;
    }
};

template <typename _T, typename _U, typename _Entry, _Entry _U::*__field>
struct bsdlist<_T, _U, _Entry, __field, ListType::slist, true> : private bsdlist<typename is_ptr<_T>::type, _U, _Entry, __field, ListType::slist, false> {
public:
    typedef bsdlist<
        typename is_ptr<_T>::type, 
        _U, _Entry, __field, 
        ListType::clist, false>     base;
    typedef typename base::type     type;
    typedef _T                      pointer;
    typedef const pointer           const_pointer;

    using base::swap;
    using base::operator=;
    using base::first;
    using base::front;
    using base::next;
    using base::empty;
    using base::iterator;
    using base::begin;
    using base::end;
    using base::cbegin;
    using base::cend;

    bsdlist() noexcept : base() {}
    bsdlist(const bsdlist&) = delete;
    bsdlist(bsdlist &&x) noexcept : base(std::move(x)) {}

    ~bsdlist() noexcept {
        clear();
    }

    void clear() noexcept {
        while (!empty()) {
            pop_front();
        }
    }

    pointer insert_back(type *listelm, const_pointer &elm) noexcept {
        elm.retain();
        base::insert_back(listelm, elm);
        return elm;
    }

    pointer remove(type *elm, type *prev) noexcept {
        pointer ret;
        ret._ptr = base::remove(elm, prev);
        return ret;
    }

    pointer remove(type *elm) noexcept {
        pointer ret;
        ret._ptr = base::remove(elm);
        return ret;
    }

    pointer push_front(const_pointer &elm) noexcept {
        elm.retain();
        base::push_front(elm);
        return elm;
    }

    pointer pop_front() noexcept {
        pointer ret;
        ret._ptr = base::pop_front();
        return ret;
    }
};

template <typename _T, typename _U, typename _Entry, _Entry _U::*__field>
struct bsdlist<_T, _U, _Entry, __field, ListType::stlist, true> : private bsdlist<typename is_ptr<_T>::type, _U, _Entry, __field, ListType::stlist, false> {
public:
    typedef bsdlist<
        typename is_ptr<_T>::type, 
        _U, _Entry, __field, 
        ListType::clist, false>     base;
    typedef typename base::type     type;
    typedef _T                      pointer;
    typedef const pointer           const_pointer;

    using base::swap;
    using base::operator=;
    using base::first;
    using base::front;
    using base::next;
    using base::empty;
    using base::iterator;
    using base::begin;
    using base::end;
    using base::cbegin;
    using base::cend;

    bsdlist() noexcept : base() {}
    bsdlist(const bsdlist&) = delete;
    bsdlist(bsdlist &&x) noexcept : base(std::move(x)) {}

    ~bsdlist() noexcept {
        clear();
    }

    void clear() noexcept {
        while (!empty()) {
            pop_front();
        }
    }

    static pointer insert_back(type *listelm, const_pointer &elm) noexcept {
        elm.retain();
        base::insert_back(listelm, elm);
        return elm;
    }

    pointer remove(type *elm, type *prev) noexcept {
        pointer ret;
        ret._ptr = base::remove(elm, prev);
        return ret;
    }

    pointer remove(type *elm) noexcept {
        pointer ret;
        ret._ptr = base::remove(elm);
        return ret;
    }

    pointer push_front(const_pointer &elm) noexcept {
        elm.retain();
        base::push_front(elm);
        return elm;
    }

    pointer pop_front() noexcept {
        pointer ret;
        ret._ptr = base::pop_front();
        return ret;
    }

    pointer push_back(const_pointer &elm) noexcept {
        elm.retain();
        base::push_back(elm);
        return elm;
    }
};

GV_NS_END

#define gv_list(_T, entry) GV_NS::bsdlist<        \
    _T,                                           \
    typename GV_NS::member_of<                    \
        decltype(&GV_NS::is_ptr<_T>::type::entry) \
    >::class_type,                                \
    typename GV_NS::member_of<                    \
        decltype(&GV_NS::is_ptr<_T>::type::entry) \
    >::type,                                      \
    &GV_NS::is_ptr<_T>::type::entry,              \
    GV_NS::member_of<                             \
        decltype(&GV_NS::is_ptr<_T>::type::entry) \
    >::type::list_type,                           \
    GV_NS::is_ptr<_T>::value>

#endif

