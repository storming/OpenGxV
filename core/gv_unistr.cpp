#include "opengv.h"
#include "gv_unistr.h"

GV_NS_BEGIN

const ptr<UniStr> &UniStrPool::get(const char *str, size_t size) noexcept {
    static object<UniStr> tmp;
    if (!size) {
        size = std::strlen(str);
    }
    tmp->_string = (char*)str;
    tmp->_size = size;
    tmp->_hash = GV_NS::hash(str, size);

    auto em = _map.emplace(tmp);
    tmp->_string = nullptr;
    if (!em.second) {
        return *em.first;
    }
    object<UniStr> newstr;
    newstr->_it = em.first;
    newstr->_string = (char*)malloc(size + 1);
    memcpy((char*)(newstr->_string), str, size);
    *(newstr->_string + size) = '\0';
    newstr->_size = size;
    newstr->_hash = tmp->_hash;
    (ptr<UniStr>&)(*em.first) = std::move(newstr);
    return *em.first;
}

GV_NS_END