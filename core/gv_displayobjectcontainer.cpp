#include "opengxv.h"
#include "gv_displayobjectcontainer.h"
#include "gv_stage.h"

GV_NS_BEGIN

DisplayObjectContainer::DisplayObjectContainer() noexcept : InteractiveObject(true)
{ }

DisplayObject *DisplayObjectContainer::addChild(const ptr<DisplayObject> &child, DisplayObject *before) {
    gv_assert(child, "child is null.");
    gv_assert(!before || before->_parent == this, "before is not contains by the container.");

    if (child->_parent) {
        if (child->_parent == this) {
            if (child != before) {
                Container::remove(child); 
                if (before) {
                    Container::insert_front(before, child);
                }
                else {
                    _container.push_back(child);
                }
            }
            return child;
        }
        child->_parent->removeChild(child);
    }

    child->_parent = this;
    child->_matrixDirty = true;
    ++_container._size;

    if (before) {
        Container::insert_front(before, child);
    }
    else {
        _container.push_back(child);
    }

    if (!child->_bounds.empty()) {
        updateChildBounds(child, Box2f(), child->_bounds); 
    }
    child->dispatchEvent(object<Event>(Event::ADDED, true)); 
    if (_stage) {
        child->stage(_stage);
    }
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
    --_container._size; 
    Container::remove(child);
    child->_parent = nullptr;

    if (update && !child->_bounds.empty()) {
        updateChildBounds(child, child->_bounds, Box2f()); 
    }

    child->dispatchEvent(this, object<Event>(Event::REMOVED, true));
    if (_stage) {
        child->stage(nullptr);
    }
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
    while ((child = _container.pop_front())) {
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

    DisplayObject *next = _container.next(a); 
    if (next == b) {
        Container::remove(a);
        Container::insert_back(b, a);
    }
    else if (_container.prev(a) == b) {
        Container::remove(b);
        Container::insert_back(a, b);
    }
    else {
        Container::remove(a);
        Container::insert_back(b, a);
        Container::remove(b);
        Container::insert_front(next, b);
    }
}

void DisplayObjectContainer::swapChildren(unsigned a, unsigned b) {
    if (a == b) {
        return;
    }

    if (a < b) {
        std::swap(a, b);
    }

    gv_assert(a < _container._size, "child index out of range.");

    DisplayObject *child_a = nullptr, *child_b = nullptr;
    unsigned n = 0;
    for (auto child : _container) {
        if (n == b) {
            child_b = child;
        }
        if (n == a) {
            child_a = child;
            break;
        }
        n++;
    }

    swapChildren(child_a, child_b);
}

unsigned DisplayObjectContainer::getChildIndex(DisplayObject *which) {
    gv_assert(which->_parent == this, "child is not contains by the container.");

    unsigned n = 0;
    for (auto child : _container) {
        if (child == which) {
            break;
        }
        n++;
    }
    return n;
}

void DisplayObjectContainer::setChildIndex(DisplayObject *child, unsigned index) {
    addChild(child, index);
}

DisplayObject *DisplayObjectContainer::getChildAt(unsigned index) {
    gv_assert(index < _container._size, "index out of range.");
    for (auto child : _container) {
        if (!index--) {
            return child;
        }
    }
    return nullptr;
}

DisplayObject *DisplayObjectContainer::getChildByName(UniStr *name, bool recursive) {
    for (auto child : _container) {
        if (child->_name == name) {
            return child;
        }
        if (recursive && child->_iscontainer) {
            DisplayObject *c = static_cast<DisplayObjectContainer*>(child)->getChildByName(name, recursive);
            if (c) {
                return c;
            }
        }
    }
    return nullptr;
}

bool DisplayObjectContainer::contains(DisplayObject *which) {
    for (auto child : _container) {
        if (child == which) {
            return true;
        }
        if (child->_iscontainer) {
            bool b = static_cast<DisplayObjectContainer*>(child)->contains(which);
            if (b) {
                return b;
            }
        }
    }
    return false;
}

void DisplayObjectContainer::bringChildToFront(const ptr<DisplayObject> &child) {
    gv_assert(child->_parent == this, "child is not contains by the container.");

    Container::remove(child);
    _container.push_back(child);
}

void DisplayObjectContainer::sendChildToBack(const ptr<DisplayObject> &child) {
    gv_assert(child->_parent == this, "child is not contains by the container.");

    Container::remove(child);
    _container.push_front(child);
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
        for (auto child : _container) {
            const Box2f &childBounds = child == which ? newBounds : child->_bounds;
            _childrenBounds |= childBounds;
        }
    }

    if (bounds != _childrenBounds) {
        _childrenBounds = bounds;
        updateBounds();
    }
}

void DisplayObjectContainer::stage(Stage *stage) {
    DisplayObject::stage(stage);
    for (auto child : _container) {
        child->stage(stage);
    }
}

void DisplayObjectContainer::render(Renderer &renderer, const Matrix &mat, int dirty) noexcept {
    if (!_visible) {
        return;
    }
    dirty |= (int)_matrixDirty; 
    if (dirty) {
        *_concatenatedMatrix = mat * (*_matrix);
        _matrixDirty = false;
    }
    if (!_stage->checkVisibility((*_concatenatedMatrix) * _bounds)) {
        return;
    }

    draw(renderer, *_concatenatedMatrix);

    for (ptr<DisplayObject> child : _container) {
        if (child->_iscontainer) {
            static_cast<DisplayObjectContainer*>(child.get())->render(renderer, *_concatenatedMatrix, dirty);
        }
        else {
            child->render(renderer, *_concatenatedMatrix, dirty);
        }
    }
}

GV_NS_END
