#include "opengv.h"
#include "gv_unistr.h"

GV_NS_BEGIN

UniStr::UniStr(const char *str, size_t size) noexcept : _chunk(str, size + 1) {
    _chunk[size] = '\0';
}

UniStr::~UniStr() noexcept {
    UniStrPool::instance()->_map.erase(this);
}

ptr<UniStr> UniStrPool::get(const char *str, size_t size) noexcept {
    if (!size) {
        size = std::strlen(str);
    }

    ptr<UniStr> tmp;
    auto em = _map.emplace(std::pair<const char*, size_t>(str, size), [&](){
        return tmp = object<UniStr>(str, size);
    });
    return std::addressof<UniStr>(*em.first);
}

GV_NS_END