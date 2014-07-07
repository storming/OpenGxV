#include "opengxv.h"
#include "gv_displayobject.h"
#include "gv_displayobjectcontainer.h"

GV_NS_BEGIN

DisplayObject::DisplayObject() : _flags() { 
    _transform.makeAffine();
}

void DisplayObject::size(const Size2f &value) {
    if (!_bounds) {
        return;
    }
    Size2f boundsSize = _bounds.size();
    if (value == boundsSize) {
        return;
    }
    float sx = value.width / boundsSize.width;
    float sy = value.height / boundsSize.height;
    _transform = _transform * Scaling(sx, sy, 0.f);
    transform(_transform);
}

void DisplayObject::width(float value) {
    size(Size2f(value, height()));
}

void DisplayObject::height(float value) {
    size(Size2f(width(), value));
}

void DisplayObject::position(const Vec3f &value) {
    Vec3f d = value - _transform.translation();
    _transform.translation() = value;
    _bounds += Vec2f(d.x(), d.y());
    _flags |= TRANSFORM_DIRTY;
}

void DisplayObject::x(float value) {
    float d = value - _transform.translation().x();
    _transform.translation().x() = value;
    _bounds.min.x() += d;
    _bounds.max.x() += d;
    _flags |= TRANSFORM_DIRTY;
}

void DisplayObject::y(float value) {
    float d = value - _transform.translation().y();
    _transform.translation().y() = value;
    _bounds.min.y() += d;
    _bounds.max.y() += d;
    _flags |= TRANSFORM_DIRTY;
}

void DisplayObject::z(float value) {
    _transform.translation().z() = value;
    _flags |= TRANSFORM_DIRTY;
}

Vec3f DisplayObject::rotation() const {
    Vec3f rot;
    linear(&rot, nullptr);
    return math::angle(rot);
}
void DisplayObject::rotation(const Vec3f &value) {
    Matrix3f scale;
    linear((Vec3f*)nullptr, &scale);
    _transform.linear() = math::rotation(math::radian(value)) * scale;
    transform(_transform);
}

float DisplayObject::rotationX() const {
    return rotation().x();
}

void DisplayObject::rotationX(float value) {
    Vec3f rot;
    Matrix3f scale;
    linear(&rot, &scale);
    rot.x() = math::radian(value);
    _transform.linear() = math::rotation(rot) * scale;
    transform(_transform);
}

float DisplayObject::rotationY() const {
    return rotation().y();
}

void DisplayObject::rotationY(float value) {
    Vec3f rot;
    Matrix3f scale;
    linear(&rot, &scale);
    rot.y() = math::radian(value);
    _transform.linear() = math::rotation(rot) * scale;
    transform(_transform);
}

float DisplayObject::rotationZ() const {
    return rotation().z();
}

void DisplayObject::rotationZ(float value) {
    Vec3f rot;
    Matrix3f scale;
    linear(&rot, &scale);
    rot.z() = math::radian(value);
    _transform.linear() = math::rotation(rot) * scale;
    transform(_transform);
}

Vec3f DisplayObject::scale() const {
    Matrix3f m;
    linear((Matrix3f*)nullptr, &m);
    return Vec3f(m(0, 0), m(1, 1), m(2, 2));
}
void DisplayObject::scale(const Vec3f &value) {
    Matrix3f rot;
    linear(&rot, nullptr);
    _transform.linear() = rot * Scaling(value);
    transform(_transform);
}
float DisplayObject::scaleX() const {
    return scale().x();
}
void DisplayObject::scaleX(float value) {
    Matrix3f rot;
    linear(&rot, nullptr);
    _transform.linear() = rot * Scaling(value, 0.f, 0.f);
    transform(_transform);
}

float DisplayObject::scaleY() const {
    return scale().y();
}
void DisplayObject::scaleY(float value) {
    Matrix3f rot;
    linear(&rot, nullptr);
    _transform.linear() = rot * Scaling(0.f, value, 0.f);
    transform(_transform);
}
float DisplayObject::scaleZ() const {
    return scale().z();
}
void DisplayObject::scaleZ(float value) {
    Matrix3f rot;
    linear(&rot, nullptr);
    _transform.linear() = rot * Scaling(0.f, 0.f, value);
    transform(_transform);
}

void DisplayObject::linear(Vec3f *rot, Matrix3f *scale) const {
    Matrix3f m;
    linear(&m, scale);
    *rot = m.eulerAngles(0, 1, 2);
}

void DisplayObject::updateBounds() {
    _flags &= ~BOUNDS_DIRTY;
    if (!_bounds) {
        return;
    }
    Box2f b = contentBounds();

    Affine2f t;
    const Transform &trans = globalTransform();
    t.matrix().topLeftCorner(2, 2) = trans.matrix().topLeftCorner(2, 2);
    t.matrix().topRightCorner(2, 1) = trans.matrix().topRightCorner(2, 1);
    Vec2f c1(b.min.x(), b.max.y());
    Vec2f c2(b.max.x(), b.min.y());
    Box2f bounds;
    bounds.min = _bounds.max = t * b.min;
    bounds |= t * b.min;
    bounds |= t * c1;
    bounds |= t * c2;

    if (_bounds != bounds && _parent) {
        _parent->updateBounds(this, &bounds);
    }
    _bounds = bounds;
}

bool DisplayObject::attach(DisplayObjectContainer *container) noexcept {
    gv_assert(!_parent, "child already has parent.");
    _parent = container;
    _flags |= (TRANSFORM_DIRTY | BOUNDS_DIRTY);
    return true;
}

void DisplayObject::dettach(DisplayObjectContainer *container) noexcept {
    _parent = nullptr;
}

void DisplayObject::bringToFront() noexcept {
    if (!_parent) {
        return;
    }
    _parent->_list.push_front(_parent->_list.remove(this));
}

void DisplayObject::sendToBack() noexcept {
    if (!_parent) {
        return;
    }
    _parent->_list.push_back(_parent->_list.remove(this));
}

bool DisplayObject::dispatchEvent(ptr<Event> event) {
    static std::vector<ptr<EventDispatcher>> dispatchers;
    size_t old_size = dispatchers.size();
    DisplayObject *parent = _parent; 
    while (parent) {
        dispatchers.emplace_back(parent);
        parent = parent->_parent;
    }
    bool ret = EventDispatcher::dispatchEvent(event, this, dispatchers.data() + old_size, dispatchers.size() - old_size);
    dispatchers.resize(old_size);
    return ret;
}

GV_NS_END


