#include "opengv.h"
#include "gv_object.h"
#include "gv_log.h"

GV_NS_BEGIN

Object::singletons Object::_singletons;
int Object::_constructRef = 0;
int Object::_destroyRef = 0;

Object::singletons::~singletons() {
    while (!_stack.empty()) {
        Object *obj = _stack.top();
        _stack.pop();
        destroyObject(obj);
    }
}

void Object::constructFailed() {
    gv_fail("can't 'new' class Object, must use object template to construct it.");
}

void Object::destroyFailed() {
    gv_fail("can't 'delete' class Object.");
}

GV_NS_END