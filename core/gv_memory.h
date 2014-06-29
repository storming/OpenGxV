#ifndef __GV_MEMORY_H__
#define __GV_MEMORY_H__


#include "gv_platform.h"

GV_NS_BEGIN

inline void *mem_alloc(size_t size) noexcept {
    return std::malloc(size);
}

inline void mem_free(void *p, size_t size) noexcept {
    return std::free(p);
}

GV_NS_END

#endif

