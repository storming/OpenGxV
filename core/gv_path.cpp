#include "opengxv.h"
#include "gv_path.h"

#define GETC() do {                \
    c = str < end ? *str++ : '\0'; \
} while (0)

GV_NS_BEGIN

inline ptr<Path> PathPool::probe(ptr<Path> &parent, const char *name, unsigned len) noexcept {
    ptr<UniStr> str = gv_unistr(name, len);
    ptr<Path> path;
    auto em = _map.emplace(str, [&]() {
        path = object<Path>();
        path->_name = str;
        path->_parent = parent;
        return path;
    });
    return std::addressof<Path>(*em.first);
}

inline const ptr<Path> &PathPool::current() noexcept {
    if (_stack.empty()) {
        return _root;
    }
    return _stack.top();
}

ptr<Path> PathPool::get(const char *str, size_t len) noexcept {
    int c;
    if (!len) {
        len = strlen(str);
    }
    const char *end = str + len;
    while (end > str) {
        if (!isspace(*(end - 1))) {
            break;
        }
        end--;
    }
    GETC();
    while (c && isspace(c)) {
        GETC();
    }

    ptr<Path> &cur = _root;
    if (c == '/') {
        GETC();
    }
    else {
        cur = current();
    }
    const char *start = str - 1;
    while (c) {
        if (c != '/') {
            GETC();
            continue;
        }
        int len = str - start - 1;
        switch (len) {
        case 0:
            start = str;
            GETC();
            continue;
        case 1:
            if (*start == '.') {
                start = str;
                GETC();
                continue;
            }
            break;
        case 2:
            if (*start == '.' && *(start + 1) == '.') {
                cur = cur->_parent;
                if (!cur) {
                    cur = _root;
                    goto finish;
                }
                start = str;
                GETC();
                continue;;
            }
            break;
        default:
            break;
        }
        cur = probe(cur, start, len);
        start = str;
        GETC();
    }

    if (start < end) {
        cur = probe(cur, start, end - start);
    }
finish:
    return cur;
}

std::string Path::name() const {
    std::string ret;
    if (this != PathPool::instance()->_root) {
        const char *p, *name;
        name = p = _name->c_str();
        const char *end = p + _name->size();
        while (p < end && *p != '.') {
            p++;
        }
        ret.assign(name, p - name);
    }
    return ret;
}

std::string Path::ext() const {
    std::string ret;
    if (this != PathPool::instance()->_root) {
        const char *p = _name->c_str();
        const char *end = p + _name->size();
        while (p < end && *p++ != '.');
        if (p < end) {
            ret.assign(p, end - p);
        }
    }
    return ret;
}

std::string Path::tostring() const {
    Path *stack[128], *entry;
    Path **stack_p = stack;
    entry = (Path*)this;
    while (entry) {
        *stack_p++ = entry;
        entry = entry->_parent;
    }

    std::string ret;
    --stack_p; 
    bool first = true;
    while (--stack_p >= stack) {
        entry = *stack_p;
        if (first) {
            first = false;
        }
        else {
            ret.append("/", 1);
        }
        ret.append(entry->_name->c_str(), entry->_name->size());
    }
    return ret;
}

GV_NS_END


