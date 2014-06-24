#ifndef __GV_SINGLETON_H__
#define __GV_SINGLETON_H__

#include "gv_object.h"

GV_NS_BEGIN

#define GV_FRIEND_SINGLETON() template <typename, typename...> friend class gv::singleton

template <typename _T, typename ..._Deps>
class singleton {
private:
    template <size_t __n, typename _D> struct trace_deps {
        static bool check() {
            return true;
        }
    };

    template <size_t __n, typename _D, typename ..._Others>
    struct trace_deps<__n, std::tuple<_D, _Others...>> {
        static bool check() {
            if (!std::is_same<_T, _D>::value) {
                if (!_D::instance()) {
                    return false;
                }
            }
            return trace_deps<__n - 1, std::tuple<_Others...>>::check();
        }
    };

    template <typename _D, typename ..._Others>
    struct trace_deps<1, std::tuple<_D, _Others...>> {
        static bool check() {
            if (!std::is_same<_T, _D>::value) {
                if (!_D::instance()) {
                    return false;
                }
            }
            return true;
        }
    };

    static _T *construct_object();
public:
	static _T *instance() {
		static _T *obj = nullptr;
        if (!obj) {
            obj = construct_object();
        }
		return obj;
	}
};

template <typename _Tx, typename ..._Deps>
_Tx *singleton<_Tx, _Deps...>::construct_object() {
    if (!trace_deps<sizeof...(_Deps), std::tuple<_Deps...>>::check()) {
        return nullptr;
    }
    _Tx *obj = new _Tx();
    if (!obj) {
        return nullptr;
    }
    if (!obj->init()) {
        delete obj;
        return nullptr;
    }
    Object::_singletons._stack.push(obj);
    return obj;
}

GV_NS_END

#endif

