//#if 1
#ifndef __GV_RBMAP_H__
#define __GV_RBMAP_H__

#include "gv_object.h"
#include "gv_rbtree.h"

GV_NS_BEGIN

struct rbmap_entry : rbtree::node {};
typedef rbmap_entry map_entry;

template <
    typename _T,
    typename _U,
    typename _Entry,
    _Entry _U::*__field>
struct rbmap_traits {
    static_assert(std::is_same<_U, _T>::value || std::is_base_of<_U, _T>::value,
                  "rbmap _T must same with _U or derived from _U");
    typedef _T*         pointer;
    typedef const _T*   const_pointer;
    typedef _T&         reference;
    typedef _Entry      entry_type;

    static pointer get_pointer(entry_type *entry) noexcept {
        return static_cast<pointer>(containerof_member(entry, __field));
    }
    static pointer get_pointer(rbtree::node *node) noexcept {
        return get_pointer(static_cast<entry_type*>(node));
    }
    static entry_type *get_entry(_U *p) noexcept {
        return &(p->*__field);
    }
};

template <typename _Entry, bool>
struct rbmap_left_acc_policy {
    typedef _Entry entry_type;

    rbmap_left_acc_policy() noexcept : _left() {}
    void set_left(entry_type *entry) noexcept {
        _left = entry;
    }

    void test_left(entry_type *entry) noexcept {
        if (_left == entry) {
            _left = static_cast<entry_type*>(_left->next());
        }
    }

    entry_type *get_left(rbtree *tree) noexcept {
        return _left;
    }

    entry_type *_left;
};

template <typename _Entry>
struct rbmap_left_acc_policy<_Entry, false> {
    typedef _Entry entry_type;

    void set_left(entry_type *entry) noexcept {
    }

    void test_left(entry_type *entry) noexcept {
    }

    entry_type *get_left(rbtree *tree) noexcept {
        return static_cast<entry_type*>(tree->front());
    }
};

template <typename _Entry, bool>
struct rbmap_right_acc_policy {
    typedef _Entry entry_type;

    rbmap_right_acc_policy() noexcept : _right() {}
    void set_right(entry_type *entry) noexcept {
        _right = entry;
    }

    void test_right(entry_type *entry) noexcept {
        if (_right == entry) {
            _right = static_cast<entry_type*>(_right->prev());
        }
    }

    entry_type *get_right(rbtree *tree) noexcept {
        return _right;
    }

    entry_type *_right;
};

template <typename _Entry>
struct rbmap_right_acc_policy<_Entry, false> {
    typedef _Entry entry_type;

    void set_right(entry_type *entry) noexcept {
    }

    void test_right(entry_type *entry) noexcept {
    }

    entry_type *get_right(rbtree *tree) noexcept {
        return static_cast<entry_type*>(tree->back());
    }
};

template <typename _Traits>
struct rbmap_iterator {
    typedef _Traits                             traits_type;
    typedef typename traits_type::pointer       pointer;
    typedef typename traits_type::reference     reference;
    typedef typename traits_type::entry_type    entry_type;

    rbmap_iterator() noexcept : _entry() {}
    rbmap_iterator(entry_type *entry) noexcept : _entry(entry) {}
    rbmap_iterator(const rbmap_iterator &x) noexcept : _entry(x._entry) {}
    rbmap_iterator(pointer x) noexcept : _entry(traits_type::get_entry(x)) {}
    rbmap_iterator(rbtree::node *x) noexcept : _entry(static_cast<entry_type*>(x)) {}

    reference operator*() const noexcept {
        return *traits_type::get_pointer(_entry);
    }
    pointer operator->() const noexcept {
        return traits_type::get_pointer(_entry);
    }
    rbmap_iterator operator++() noexcept {
        _entry = static_cast<entry_type*>(_entry->next());
        return *this;
    }
    rbmap_iterator operator++(int) noexcept {
        iterator tmp(*this);
        _entry = static_cast<entry_type*>(_entry->next());
        return tmp;
    }
    rbmap_iterator operator--() noexcept {
        _entry = static_cast<entry_type*>(_entry->prev());
        return *this;
    }
    rbmap_iterator operator--(int) noexcept {
        iterator tmp(*this);
        _entry = static_cast<entry_type*>(_entry->prev());
        return tmp;
    }
    bool operator==(const rbmap_iterator &rhs) const noexcept {
        return _entry == x._entry;
    }
    bool operator!=(const rbmap_iterator &rhs) const noexcept {
        return _entry != x._entry;
    }

    entry_type *_entry;
};

template <typename _Traits>
struct rbmap_const_iterator {
    typedef _Traits                                 traits_type;
    typedef const typename traits_type::pointer     pointer;
    typedef const typename traits_type::reference   reference;
    typedef const typename traits_type::entry_type  entry_type;

    rbmap_const_iterator() noexcept : _entry() {}
    rbmap_const_iterator(entry_type *entry) noexcept : _entry(entry) {}
    rbmap_const_iterator(const rbmap_const_iterator &x) noexcept : _entry(x._entry) {}
    rbmap_const_iterator(const rbtree::node *x) noexcept : _entry(static_cast<entry_type*>(x)) {}

    reference operator*() const noexcept {
        return *get_object(_entry);
    }
    pointer operator->() const noexcept {
        return get_object(_entry);
    }
    rbmap_const_iterator operator++() noexcept {
        _entry = static_cast<entry_type*>(_entry->next());
        return *this;
    }
    rbmap_const_iterator operator++(int) noexcept {
        iterator tmp(*this);
        _entry = static_cast<entry_type*>(_entry->next());
        return tmp;
    }
    rbmap_const_iterator operator--() noexcept {
        _entry = static_cast<entry_type*>(_entry->prev());
        return *this;
    }
    rbmap_const_iterator operator--(int) noexcept {
        iterator tmp(*this);
        _entry = static_cast<entry_type*>(_entry->prev());
        return tmp;
    }
    bool operator==(const rbmap_const_iterator &rhs) const noexcept {
        return _entry == x._entry;
    }
    bool operator!=(const rbmap_const_iterator &rhs) const noexcept {
        return _entry != x._entry;
    }

    entry_type *_entry;
};

template <
    typename _Key,
    typename _T,
    typename _U,
    typename _Entry,
    _Entry _U::*__field,
    typename _Compare,
    bool __left_acc = false,
    bool __right_acc = false>
class rbmap : 
    protected rbtree, 
    protected rbmap_left_acc_policy<_Entry, __left_acc>,
    protected rbmap_right_acc_policy<_Entry, __right_acc> {
public:
    typedef rbmap_traits<_T, _U, _Entry, __field>   traits_type;
    typedef _Entry                                  entry_type;
    typedef _T*                                     pointer;
    typedef const _T*                               const_pointer;
    typedef _T&                                     reference;
    typedef _Key                                    key_type;
    typedef _Compare                                compare_type;
    typedef rbmap_iterator<traits_type>             iterator;
    typedef rbmap_const_iterator<traits_type>       const_iterator;
public:
    rbmap(compare_type compare = compare_type()) noexcept : _compare(compare) {}
    void clear() noexcept {
        init();
    }

    template <typename _Destroy>
    void clear(_Destroy &&destroy) noexcept {
        if (_root) {
            remove_tree(static_cast<entry_type *>(_root), std::forward<_Destroy>(destroy)); 
        }
    }

    template <typename _Construct, typename ..._Args>
    std::pair<iterator, bool> emplace(const key_type &key, _Construct construct, _Args&&...args) noexcept {
        rbtree::node **link = &_root;
        rbtree::node *parent = nullptr;
        int GV_UNUSED(left) = 1;
        int GV_UNUSED(right) = 1;

        pointer p;
        int cmp;

        while (*link) {
            parent = *link;
            p = traits_type::get_pointer(parent);

            cmp = _compare(key, *p);
            if (cmp < 0) {
                link = &(*link)->_left;
                if (__right_acc) {
                    right = 0;
                }            }
            else if (cmp > 0) {
                link = &(*link)->_right;
                if (__left_acc) {
                    left = 0;
                }            
            }
            else {
                return std::pair<iterator, bool>(iterator(parent), false);
            }
        }

        p = construct(std::forward<_Args>(args)...);
        entry_type *entry = traits_type::get_entry(p);
        rbtree::link(entry, parent, link); 
        insert(entry);
        if (__left_acc && left) {
            set_left(entry);
        }
        if (__right_acc && right) {
            set_right(entry);
        }
        return std::pair<iterator, bool>(iterator(entry), true);
    }

    template <typename _AKey, typename _ACompare>
    iterator find(_AKey &&key, _ACompare &&compare) noexcept {
        rbtree::node *node = _root;
        pointer p;
        int cmp;
        while (node) {
            p = traits_type::get_pointer(node);
            cmp = compare(std::forward<_AKey>(key), *p);
            if (cmp < 0) {
                node = node->_left;
            }
            else if (cmp > 0) {
                node = node->_right;
            }
            else {
                return iterator(static_cast<entry_type*>(node));
            }
        }
        return end();
    }

    template <typename _AKey, typename _ACompare>
    const_iterator find(_AKey &&key, _ACompare &&compare) const noexcept {
        rbtree::node *node = _root;
        pointer p;
        int cmp;
        while (node) {
            p = traits_type::get_pointer(node);
            cmp = compare(std::forward<_AKey>(key), *p);
            if (cmp < 0) {
                node = node->_left;
            }
            else if (cmp > 0) {
                node = node->_right;
            }
            else {
                return iterator(static_cast<entry_type*>(node));
            }
        }
        return end();
    }

    iterator find(const key_type &key) noexcept {
        return find(key, _compare);
    }

    const_iterator find(const key_type &key) const noexcept {
        return find(key, _compare);
    }

    pointer erase(pointer elm) noexcept {
        entry_type *entry = traits_type::get_entry(elm);
        if (__left_acc) {
            test_left(entry);
        }
        if (__right_acc) {
            test_right(entry);
        }
        remove(entry);
        return elm;
    }
    pointer erase(iterator &it) noexcept {
        return erase(std::address_of<pointer>(*it));
    }
    pointer erase(const key_type &key) noexcept {
        return erase(find(key));
    }
    pointer replace(const key_type &key, pointer new_elm) noexcept {
        rbtree::node **link = &_root;
        rbtree::node *parent = nullptr;
        int GV_UNUSED(left) = 1;
        int GV_UNUSED(right) = 1;

        pointer p;
        int cmp;

        while (*link) {
            parent = *link;
            p = traits_type::get_pointer(parent);

            cmp = _compare(key, *p);
            if (cmp < 0) {
                link = &(*link)->_left;
                if (__right_acc) {
                    right = 0;
                }            }
            else if (cmp > 0) {
                link = &(*link)->_right;
                if (__left_acc) {
                    left = 0;
                }            
            }
            else {
                entry_type *entry = traits_type::get_entry(new_elm);
                replace(parent, entry);
                if (__left_acc && left && !entry->_left) {
                    set_left(entry);
                }
                if (__right_acc && right && !entry->_right) {
                    set_right(entry);
                }
                return p;
            }
        }

        p = new_elm;
        entry_type *entry = traits_type::get_entry(p);
        rbtree::link(entry, parent, link); 
        insert(entry);
        if (__left_acc && left) {
            set_left(entry);
        }
        if (__right_acc && right) {
            set_right(entry);
        }
        return nullptr;
    }
    pointer replace(iterator &it, pointer new_elm) noexcept {
        return replace(std::address_of<pointer*>(*it), new_elm);
    }
    pointer front() const noexcept {
        return traits_type::get_pointer(get_left(this));
    }
    pointer back() const noexcept {
        return traits_type::get_pointer(get_right(this));
    }
    static pointer next(const_pointer elm) noexcept {
        entry_type *entry = traits_type::get_entry((pointer)elm);
        rbtree::node *node = entry->next();
        if (!node) {
            return nullptr;
        }
        return traits_type::get_pointer(node);
    }
    static pointer prev(const_pointer elm) noexcept {
        entry_type *entry = traits_type::get_entry((pointer)elm);
        rbtree::node *node = entry->prev();
        if (!node) {
            return nullptr;
        }
        return traits_type::get_pointer(node);
    }
    iterator begin() noexcept {
        return iterator(get_left(this));
    }
    const iterator begin() const noexcept {
        return const_iterator(get_left(this));
    }
    const iterator cbegin() const noexcept {
        return const_iterator(get_left(this));
    }
    iterator end() noexcept {
        return iterator((entry_type*)nullptr);
    }
    const_iterator end() const noexcept {
        return iterator((entry_type*)nullptr);
    }
    const_iterator cend() const noexcept {
        return iterator((entry_type*)nullptr);
    }
private:
    template <typename _Destroy>
    static void remove_tree(entry_type *entry, _Destroy &&destroy) noexcept {
        if (entry->_left) {
            remove_tree(static_cast<entry_type*>(entry->_left), std::forward<_Destroy>(destroy));
        }
        if (entry->_right){
            remove_tree(static_cast<entry_type*>(entry->_right), std::forward<_Destroy>(destroy));
        }
        destroy(traits_type::get_pointer(entry));
    }
    
protected:
    compare_type _compare;
};

template <
    typename _Key,
    typename _T,
    typename _U,
    typename _Entry,
    _Entry _U::*__field,
    typename _Compare,
    bool __left_acc,
    bool __right_acc>
class rbmap<_Key, ptr<_T>, _U, _Entry, __field, _Compare, __left_acc, __right_acc> :
    protected rbmap<_Key, _T, _U, _Entry, __field, _Compare, __left_acc, __right_acc> {
public:
    typedef rbmap<
        _Key, _T, _U, _Entry, __field, 
        _Compare, __left_acc, __right_acc>  base;
    typedef typename base::compare_type     compare_type;
    typedef typename base::iterator         iterator;
    typedef _Key         key_type;
    typedef ptr<_T>                         pointer;
    using base::begin;
    using base::cbegin;
    using base::end;
    using base::cend;

    rbmap(compare_type compare = compare_type()) noexcept : base(compare) {}
    ~rbmap() noexcept {
        clear();
    }
    template <typename _Construct, typename ..._Args>
    std::pair<iterator, bool> emplace(const key_type &key, _Construct &&construct, _Args&&...args) noexcept {
        return base::emplace(
            key, 
            constructor(std::forward<_Construct>(construct)),
            std::forward<_Args>(args)...);
    }
    pointer erase(typename base::pointer elm) noexcept {
        pointer tmp;
        tmp._ptr = base::erase(elm);
        return tmp;
    }
    pointer erase(iterator &it) noexcept {
        pointer tmp;
        tmp._ptr = base::erase(it);
        return tmp;
    }
    pointer erase(const key_type &key) noexcept {
        pointer tmp;
        tmp._ptr = base::erase(key);
        return tmp;
    }
    pointer replace(const key_type &key, pointer new_elm) noexcept {
        pointer::retain_ptr(new_elm);
        pointer tmp;
        tmp._ptr = replace(key, new_elm);
        return tmp;
    }
    pointer replace(iterator &it, pointer new_elm) noexcept {
        pointer::retain_ptr(new_elm);
        pointer tmp;
        tmp._ptr = replace(it, new_elm);
        return tmp;
    }
    void clear() noexcept {
        base::clear([](base::pointer elm){
            pointer::release_ptr(elm);
        });
    }
private:
    template <typename _Construct>
    struct constructor {
        const _Construct &_construct;
        constructor(const _Construct &construct) : _construct(construct) {}
        template <typename ..._Args>
        _T* operator()(_Args&&...args) {
            auto tmp = construct(std::forward<_Args>(args));
            pointer::retain_ptr(tmp);
            return tmp;
        }
    };
};

GV_NS_END

#define GV_FRIEND_RBMAP()                                                      \
template <                                                                     \
    typename _Key,                                                             \
    typename _T,                                                               \
    typename _U,                                                               \
    typename _Entry,                                                           \
    _Entry _U::*__field,                                                       \
    typename _Compare,                                                         \
    bool __left_acc,                                                           \
    bool __right_acc>                                                          \
friend class rbmap

#define gv_map(_Key, _T, __field, ...)                                         \
rbmap<                                                                         \
    _Key,                                                                      \
    _T,                                                                        \
    GV_NS::member_of<decltype(&GV_NS::is_ptr<_T>::type::__field)>::class_type, \
    GV_NS::member_of<decltype(&GV_NS::is_ptr<_T>::type::__field)>::type,       \
    &GV_NS::is_ptr<_T>::type::__field,                                         \
    ##__VA_ARGS__>

#endif

