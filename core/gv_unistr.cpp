#include "opengxv.h"
#include "gv_unistr.h"
#include "gv_memory.h"

GV_NS_BEGIN

UniStr::UniStr(const char *str, size_t size) noexcept {
    _size = size;
    _data = (char*)mem_alloc(size + 1);
    memcpy(_data, str, size);
    _data[size] = '\0';
}

UniStr::~UniStr() noexcept {
    UniStrPool::instance()->_map.erase(this);
    mem_free(_data, _size + 1);
}

ptr<UniStr> UniStrPool::get(const char *str, size_t size) noexcept {
    if (!size) {
        size = strlen(str);
    }

    ptr<UniStr> tmp;
    auto em = _map.emplace(std::pair<const char*, size_t>(str, size), [&](){
        return tmp = object<UniStr>(str, size);
    });
    return std::addressof<UniStr>(*em.first);
}

GV_NS_END