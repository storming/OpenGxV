#include "opengxv.h"
#include "gv_displayobject.h"
#include "gv_displayobjectcontainer.h"
#include "gv_stage.h"

GV_NS_BEGIN

static std::vector<ptr<DisplayObject>> __objects;

DisplayObject(bool iscontainer) noexcept
: _parent(),
  _stage(),
  _iscontainer(iscontainer),
  _visible(true),
  _transformDirty(false)
{
    _matrix = new Matrix;
    _matrix->setIdentity();
    _concatenatedMatrix = new Matrix;
}

Size2f DisplayObject::size() const {
    return _bounds.size();
}

void DisplayObject::size(const Size2f &value) {
    if (_bounds.empty()) {
        return;
    }
    Size2f boundsSize = _bounds.size();
    if (value == boundsSize) {
        return;
    }
    float sx = value.width / boundsSize.width;
    float sy = value.height / boundsSize.height;
    *_transform = (*_transform) * Scaling(sx, sy, 0.f);
    _transformDirty = true;
    updateBounds();
}

float DisplayObject::width() const {
    return size().width;
}

void DisplayObject::width(float value) {
    size(Size2f(value, height()));
}

float DisplayObject::height() const {
    return size().height;
}

void DisplayObject::height(float value) {
    size(Size2f(width(), value));
}

Vec3f DisplayObject::position() const {
    return Vec3f(_transform->translation());
}

void DisplayObject::position(const Vec3f &value) {
    auto pos = _transform->translation();
    if (pos != value) {
        Vec3f d = value - position(); 
        pos = value;
        _transformDirty = true;
        updateBounds(_bounds + Vec2f(d.x(), d.y()));
    }
}

float DisplayObject::x() const {
    return position().x();
}

void DisplayObject::x(float value) {
    auto pos = _transform->translation();
    if (pos.x() != value) {
        float d = value - pos.x();
        pos.x() = value;
        _transformDirty = true;
        updateBounds(_bounds + Vec2f(d, 0));
    }
}

float DisplayObject::y() const {
    return position().y();
}

void DisplayObject::y(float value) {
    auto pos = _transform->translation();
    if (pos.y() != value) {
        float d = value - pos.y();
        pos.y() = value;
        _transformDirty = true;
        updateBounds(_bounds + Vec2f(0, d));
    }
}

float DisplayObject::z() const {
    return position().z();
}

void DisplayObject::z(float value) {
    auto pos = _transform->translation();
    if (pos.z() != value) {
        pos.z() = value;
        _transformDirty = true;
    }
}

Vec3f DisplayObject::rotation() const {
    Vec3f rot;
    _transform->linear(&rot, nullptr);
    return math::angle(rot);
}

void DisplayObject::rotation(const Vec3f &value) {
    Matrix3f scale;
    _transform->linear((Vec3f*)nullptr, &scale);
    _transform->linear() = math::rotation(math::radian(value)) * scale;
    _transformDirty = true;
    updateBounds();
}

float DisplayObject::rotationX() const {
    return rotation().x();
}

void DisplayObject::rotationX(float value) {
    Vec3f rot;
    Matrix3f scale;
    _transform->linear(&rot, &scale);
    rot.x() = math::radian(value);
    _transform->linear() = math::rotation(rot) * scale;
    _transformDirty = true;
    updateBounds();
}

float DisplayObject::rotationY() const {
    return rotation().y();
}

void DisplayObject::rotationY(float value) {
    Vec3f rot;
    Matrix3f scale;
    _transform->linear(&rot, &scale);
    rot.y() = math::radian(value);
    _transform->linear() = math::rotation(rot) * scale;
    _transformDirty = true;
    updateBounds();
}

float DisplayObject::rotationZ() const {
    return rotation().z();
}

void DisplayObject::rotationZ(float value) {
    Vec3f rot;
    Matrix3f scale;
    _transform->linear(&rot, &scale);
    rot.z() = math::radian(value);
    _transform->linear() = math::rotation(rot) * scale;
    _transformDirty = true;
    updateBounds();
}

Vec3f DisplayObject::scale() const {
    Matrix3f m;
    _transform->linear((Matrix3f*)nullptr, &m);
    return Vec3f(m(0, 0), m(1, 1), m(2, 2));
}

void DisplayObject::scale(const Vec3f &value) {
    Matrix3f rot;
    _transform->linear(&rot, nullptr);
    _transform->linear() = rot * Scaling(value);
    _transformDirty = true;
    updateBounds();
}

float DisplayObject::scaleX() const {
    return scale().x();
}

void DisplayObject::scaleX(float value) {
    Matrix3f rot;
    _transform->linear(&rot, nullptr);
    _transform->linear() = rot * Scaling(value, 0.f, 0.f);
    _transformDirty = true;
    updateBounds();
}

float DisplayObject::scaleY() const {
    return scale().y();
}

void DisplayObject::scaleY(float value) {
    Matrix3f rot;
    _transform->linear(&rot, nullptr);
    _transform->linear() = rot * Scaling(0.f, value, 0.f);
    _transformDirty = true;
    updateBounds();
}

float DisplayObject::scaleZ() const {
    return scale().z();
}

void DisplayObject::scaleZ(float value) {
    Matrix3f rot;
    _transform->linear(&rot, nullptr);
    _transform->linear() = rot * Scaling(0.f, 0.f, value);
    _transformDirty = true;
    updateBounds();
}

bool DisplayObject::visible() const {
    return _visible;
}

void DisplayObject::visible(bool value) {
    _visible = value;
}

inline void DisplayObject::updateBounds(const Box2f &bounds) noexcept {
    if (_parent) {
        _parent->updateChildBounds(this, _bounds, bounds);
    }
    _bounds = bounds;
}

void DisplayObject::updateBounds() {
    updateBounds((*_transform) * contentBounds());
}

const Transform &DisplayObject::concatenatedTransform() noexcept {
    size_t old_size = __objects.size();
    DisplayObject *parent = _parent; 
    size_t n = -1;
    while (parent) {
        __objects.emplace_back(parent);
        if (parent->_transformDirty) {
            n = __objects.size();
        }
        parent = parent->_parent; 
    }
    Transform *trans;
    if (n != -1) {
        DisplayObject *obj = __objects[n];
        if (obj->_parent) {
            *obj->_concatenatedTransform = (*obj->_parent->_concatenatedTransform) * (*obj->_transform);
            trans = obj->_concatenatedTransform;
        }
        else {
            trans = obj->_transform;
        }
        for (--n; n <= old_size; --n) {
            obj = __objects[n];
            *obj->_concatenatedTransform = (*trans) * (*obj->_transform);
            trans = obj->_concatenatedTransform;
        }
    }
    else {
        if (!_parent) {
            trans = _transform;
            goto finish;
        }
        if (!_transformDirty) {
            trans = _concatenatedTransform;
            goto finish;
        }
        trans = _parent->_concatenatedTransform; 
    }

    *_concatenatedTransform = (*trans) * (*_transform);
    trans = _concatenatedTransform;
finish:
    __objects.resize(old_size); 
    return *trans; 
}

Box2f DisplayObject::bounds(DisplayObject *targetCoordinateSpace) {
    if (targetCoordinateSpace == _parent) {
        return _bounds;
    }

    if (targetCoordinateSpace != this) {
        if (!targetCoordinateSpace) {
            targetCoordinateSpace = Stage::instance();
        }
        if (_parent) {
            return targetCoordinateSpace->concatenatedTransform().inverse() * 
                (_parent->_concatenatedTransform() * _bounds);
        }
        else {
            return targetCoordinateSpace->concatenatedTransform().inverse() * _bounds;
        }
    }

    return contentBounds();
}

bool DisplayObject::dispatchEvent(DisplayObject *parent, ptr<Event> event) noexcept {
    size_t old_size = __objects.size();
    while (parent) {
        __objects.emplace_back(parent);
        parent = parent->_parent;
    }
    bool ret = EventDispatcher::dispatchEvent(event, this, (ptr<EventDispatcher>*)__objects.data() + old_size, __objects.size() - old_size);
    __objects.resize(old_size);
    return ret;
}

bool DisplayObject::dispatchEvent(ptr<Event> event) {
    return dispatchEvent(_parent, event);
}

GV_NS_END


