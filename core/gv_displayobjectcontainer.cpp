#include "opengv.h"
#include "gv_displayobjectcontainer.h"

GV_NS_BEGIN

DisplayObject *DisplayObjectContainer::addChild(ptr<DisplayObject> child) {
    gv_assert(child, "add child is null.");
    if (!child->attach(this)) {
        return nullptr;
    }
    _list.push_front(child);
    if (!(_flags & BOUNDS_DIRTY)) {
        _bounds |= child->bounds();
    }
    return child;
}

DisplayObject *DisplayObjectContainer::addChild(ptr<DisplayObject> child, DisplayObject *before) {
    gv_assert(child, "add child, child is null.");
    gv_assert(before, "insert after, before is null.");
    gv_assert(before->_parent == this, "insert after, before is not owned by the container.");
    if (!child->attach(this)) {
        return nullptr;
    }
    _list.insert_back(before, child);
    if (!(_flags & BOUNDS_DIRTY)) {
        _bounds |= child->bounds();
    }
    return child;
}

ptr<DisplayObject> DisplayObjectContainer::removeChild(DisplayObject *child) {
    gv_assert(child, "remove child, child is null.");
    gv_assert(child->_parent == this, "remove child, child is not owned by the container.");
    ptr<DisplayObject> ret = _list.remove(child);
    updateBounds(child, nullptr);
    child->dettach(this);
    return ret;
}

void DisplayObjectContainer::removeChildren() {
    DisplayObject *child;
    while ((child = _list.pop_front())) {
        child->dettach(this);
    }
}

void DisplayObjectContainer::swapChild(DisplayObject *a, DisplayObject *b) {
    std::swap(a->_entry, b->_entry);
}

DisplayObject *DisplayObjectContainer::child(size_t index) {
    for (auto &item : _list) {
        if (!index--) {
            return &item;
        }
    }
    return nullptr;
}

void DisplayObjectContainer::updateBounds() {
    bool first = true;
    for (auto &child : _list) {
        if (first) {
            first = nullptr;
            _bounds = child.bounds();
        }
        else {
            _bounds |= child.bounds();
        }
    }
    if (first) {
        _bounds.min.setZero();
        _bounds.max.setZero();
    }
}

void DisplayObjectContainer::updateBounds(DisplayObject *child, Box2f *newBounds) {
    if (!(_flags & BOUNDS_DIRTY) && child->_bounds.inside(_bounds)) {
        if (newBounds) {
            _bounds |= *newBounds;
        }
        return;
    }
    _flags |= BOUNDS_DIRTY;
}

GV_NS_END
