#include "opengxv.h"
#include "gv_displayobjectcontainer.h"

GV_NS_BEGIN

DisplayObjectContainer::DisplayObjectContainer() noexcept : DisplayObject(true)
{ }

DisplayObject *DisplayObjectContainer::addChild(const ptr<DisplayObject> &child, DisplayObject *before) {
    gv_assert(child, "child is null.");
    gv_assert(!before || before->_parent == this, "before is not contains by the container.");

    if (child->_parent) {
        if (child->_parent == this) {
            if (child != before) {
                ContainerType::remove(child); 
                if (before) {
                    ContainerType::insert_front(before, child);
                }
                else {
                    _list.push_back(child);
                }
            }
            return child;
        }
        child->_parent->removeChild(child);
    }

    child->_parent = this;
    child->_transformDirty = true;
    ++_list._size;

    if (before) {
        ContainerType::insert_front(before, child);
    }
    else {
        _list.push_back(child);
    }

    if (!child->_bounds.empty()) {
        updateChildBounds(child, Box2f(), child->_bounds); 
    }

    child->dispatchEvent(object<Event>(Event::ADDED, true)); 
    return child;
}

DisplayObject *DisplayObjectContainer::addChild(const ptr<DisplayObject> &child) {
    return addChild(child, nullptr);
}

DisplayObject *DisplayObjectContainer::addChild(const ptr<DisplayObject> &child, unsigned index) {
    return addChild(child, getChildAt(index));
}

ptr<DisplayObject> DisplayObjectContainer::removeChild(const ptr<DisplayObject> &child, bool update) {
    gv_assert(child, "child is null.");
    gv_assert(child->_parent == this, "child is not contains by the container.");
    --_list._size; 
    ContainerType::remove(child);
    child->_parent = nullptr;

    if (update && !child->_bounds.empty()) {
        updateChildBounds(child, child->_bounds, Box2f()); 
    }

    child->dispatchEvent(this, object<Event>(Event::REMOVED, true));
    return child;
}

ptr<DisplayObject> DisplayObjectContainer::removeChild(const ptr<DisplayObject> &child) {
    return removeChild(child, true);
}

ptr<DisplayObject> DisplayObjectContainer::removeChild(unsigned index) {
    return removeChild(getChildAt(index));
}

void DisplayObjectContainer::removeChildren() {
    ptr<DisplayObject> child;
    while ((child = _list.pop_front())) {
        removeChild(child, false);
    }
    if (!_childrenBounds.empty()) {
        _childrenBounds.setZero();
        updateBounds();
    }
}

void DisplayObjectContainer::swapChildren(const ptr<DisplayObject> &a, const ptr<DisplayObject> &b) {
    gv_assert(a && b, "child is null.");
    gv_assert(a->_parent == this && b->_parent == this, "child is not contains by the container.");

    if (a == b) {
        return;
    }

    DisplayObject *next = _list.next(a); 
    if (next == b) {
        ContainerType::remove(a);
        ContainerType::insert_back(b, a);
    }
    else if (_list.prev(a) == b) {
        ContainerType::remove(b);
        ContainerType::insert_back(a, b);
    }
    else {
        ContainerType::remove(a);
        ContainerType::insert_back(b, a);
        ContainerType::remove(b);
        ContainerType::insert_front(next, b);
    }
}

void DisplayObjectContainer::swapChildren(unsigned a, unsigned b) {
    if (a == b) {
        return;
    }

    if (a < b) {
        std::swap(a, b);
    }

    gv_assert(a < _list._size, "child index out of range.");

    DisplayObject *child_a = nullptr, *child_b = nullptr;
    DisplayObject *child = _list.last();

    unsigned n = 0;
    for (auto &child : _list) {
        if (n == b) {
            child_b = std::addressof<DisplayObject>(child);
        }
        if (n == a) {
            child_a = std::addressof<DisplayObject>(child);
            break;
        }
        n++;
    }

    swapChildren(child_a, child_b);
}

unsigned DisplayObjectContainer::getChildIndex(DisplayObject *child) {
    gv_assert(child->_parent == this, "child is not contains by the container.");

    DisplayObject *c = _list.last();
    unsigned n = 0;
    for (auto &c : _list) {
        if (std::addressof<DisplayObject>(c) == child) {
            break;
        }
    }
    return n;
}

void DisplayObjectContainer::setChildIndex(DisplayObject *child, unsigned index) {
    addChild(child, index);
}

DisplayObject *DisplayObjectContainer::getChildAt(unsigned index) {
    gv_assert(index < _list._size, "index out of range.");
    for (auto &child : _list) {
        if (!index--) {
            return std::addressof<DisplayObject>(child);
        }
    }
    return nullptr;
}

DisplayObject *DisplayObjectContainer::getChildByName(UniStr *name, bool recursive) {
    for (auto &child : _list) {
        if (child._name == name) {
            return std::addressof<DisplayObject>(child);
        }
        if (recursive && child._iscontainer) {
            DisplayObject *c = static_cast<DisplayObjectContainer&>(child).getChildByName(name, recursive);
            if (c) {
                return c;
            }
        }
    }
    return nullptr;
}

bool DisplayObjectContainer::contains(DisplayObject *which) {
    for (auto &child : _list) {
        if (std::addressof<DisplayObject>(child) == which) {
            return true;
        }
        if (child._iscontainer) {
            bool b = static_cast<DisplayObjectContainer&>(child).contains(which);
            if (b) {
                return b;
            }
        }
    }
    return false;
}

void DisplayObjectContainer::bringChildToFront(const ptr<DisplayObject> &child) noexcept {
    gv_assert(child->_parent == this, "child is not contains by the container.");

    ContainerType::remove(child);
    _list.push_back(child);
}

void DisplayObjectContainer::sendChildToBack(const ptr<DisplayObject> &child) noexcept {
    gv_assert(child->_parent == this, "child is not contains by the container.");

    ContainerType::remove(child);
    _list.push_front(child);
}

Box2f DisplayObjectContainer::contentBounds() {
    return _childrenBounds;
}

void DisplayObjectContainer::updateChildBounds(DisplayObject *which, const Box2f &oldBounds, const Box2f &newBounds) {
    Box2f bounds;

    if (oldBounds.empty() || oldBounds.inside(_bounds)) {
        bounds = _childrenBounds | newBounds;
    }
    else {
        for (auto &child : _list) {
            const Box2f &childBounds = std::addressof<DisplayObject>(child) == which ? newBounds : child._bounds;
            _childrenBounds |= childBounds;
        }
    }

    if (bounds != _childrenBounds) {
        _childrenBounds = bounds;
        updateBounds();
    }
}

GV_NS_END
