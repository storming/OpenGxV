#ifndef __GV_PATH_H__
#define __GV_PATH_H__

#include <set>
#include <stack>

#include "gv_object.h"
#include "gv_unistr.h"
#include "gv_rbmap.h"

GV_NS_BEGIN

class Path : public Object {
    friend class PathPool;
    friend class Object;
public:
    std::string name() const;
    std::string ext() const;
    std::string tostring() const;
    operator const std::string() const {
        return tostring();
    }
private:
    Path() : _parent() {}
    map_entry _entry;
    ptr<UniStr> _name;
    ptr<Path> _parent;
};

class PathPool : public Object, public singleton<PathPool, UniStrPool> {
    friend Path;
    friend class Chdir;
    friend class Object;
public:
    ptr<Path> get(const char *str, size_t len = 0) noexcept;
    ptr<Path> get(const std::string &str) noexcept {
        return get(str.c_str(), str.size());
    }
private:
    PathPool() {}
    ptr<Path> probe(ptr<Path> &parent, const char *name, unsigned len) noexcept;
    const ptr<Path> &current() noexcept;

    struct compare {
        int operator()(const ptr<UniStr> &lhs, const Path &rhs) const noexcept {
            return lhs - rhs._name;
        }
    };
    typedef gv_map(ptr<UniStr>, Path, _entry, compare) map_type;
    map_type _map;
    object<Path> _root;
    std::stack<ptr<Path>> _stack;
};

class Chdir final {
    GV_RESTRAIN_NEW();
    ptr<Path> _path;
public:
    Chdir(const ptr<Path> &x) : _path(x) {
        PathPool::instance()->_stack.push(_path);
    }
    Chdir(const char *x, size_t len = 0) : _path(PathPool::instance()->get(x, len)) {
        PathPool::instance()->_stack.push(_path);
    }
    Chdir(const std::string &x) : _path(PathPool::instance()->get(x)) {
        PathPool::instance()->_stack.push(_path);
    }
    ~Chdir() {
        PathPool::instance()->_stack.pop();
    }
};

GV_NS_END

inline GV_NS::ptr<GV_NS::Path> gv_path(const char *str, size_t size = 0) noexcept {
    return GV_NS::PathPool::instance()->get(str, size);
}

inline GV_NS::ptr<GV_NS::Path> gv_path(const std::string &str) noexcept {
    return GV_NS::PathPool::instance()->get(str);
}

#endif
