#ifndef __GV_PLATFORM_H__
#define __GV_PLATFORM_H__

#include <cstddef>
#include <type_traits>
#include <functional>

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
using std::placeholders::_4;
using std::placeholders::_5;
using std::placeholders::_6;
using std::placeholders::_7;
using std::placeholders::_8;
using std::placeholders::_9;
using std::placeholders::_10;

#define GLEW_STATIC 1
extern "C" {
#include "glew.h"
};

#if defined(WIN32) || defined(__MINGW32__)
#define NOMINMAX 1
#include <windows.h>
#if defined(WIN32)
#define noexcept throw()
#define constexpr const
#endif
#endif

#define GV_NS gv
#define GV_NS_BEGIN namespace GV_NS {
#define GV_NS_END }

#define GV_MATH_NS math
#define GV_NS_MATH_BEGIN namespace GV_MATH_NS {
#define GV_NS_MATH_END }

#define GV_INTERNAL_NS internal
#define GV_INTERNAL_BEGIN namespace GV_INTERNAL_NS {
#define GV_INTERNAL_END }

#define GV_RESTRAIN_NEW()                                  \
private:                                                   \
static void *operator new(std::size_t size) {              \
    return ::operator new(size);                           \
}                                                          \
static void *operator new[](std::size_t size) {            \
    return ::operator new[](size);                         \
}

#ifndef GV_DEBUG
#if !defined(NDEBUG)
#define GV_DEBUG 1
#endif
#else
#if !GV_DEBUG
#undef GV_DEBUG
#endif
#endif


GV_NS_BEGIN

template <typename _T>
struct member_of;

template <typename _T, typename _U>
struct member_of<_T _U::*> {
    typedef _T type;
    typedef _U class_type;
};

template <typename _T, typename _C>
inline constexpr unsigned offsetof_member(_T _C::*member) {
    static_assert(std::is_member_object_pointer<decltype(member)>::value,
        "offsetof_member only use for member object pointer.");
    return reinterpret_cast<uintptr_t>(&(((_C*)0)->*member));
};

template <typename _T, typename _C>
inline _C *containerof_member(_T *ptr, _T _C::*member) {
    return reinterpret_cast<_C*>(reinterpret_cast<char*>(ptr)-offsetof_member(member));
}

GV_NS_END

#endif