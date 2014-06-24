#include "opengv.h"
#include "gv_object.h"

GV_NS_BEGIN

Object::singletons Object::_singletons;

Object::singletons::~singletons() {
    while (!_stack.empty()) {
        Object *obj = _stack.top();
        _stack.pop();
        delete obj;
    }
}

GV_NS_END