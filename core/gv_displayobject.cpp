#include "opengxv.h"
#include "gv_displayobject.h"
#include "gv_displayobjectcontainer.h"
#include "gv_stage.h"

namespace gv {

DisplayObject::DisplayObject(bool iscontainer) noexcept
: _parent(),
  _stage(),
  _iscontainer(iscontainer),
  _visible(true),
  _matrixDirty(false)
{
    _matrix = new Matrix;
    _matrix->setIdentity();
    _concatenatedMatrix = new Matrix;
}

static std::vector<ptr<DisplayObject>> __objects;

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
    *_matrix = (*_matrix) * Scaling(sx, sy, 0.f);
    _matrixDirty = true;
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
    return Vec3f(_matrix->translation());
}

void DisplayObject::position(const Vec3f &value) {
    auto pos = _matrix->translation();
    if (pos != value) {
        Vec3f d = value - position(); 
        pos = value;
        _matrixDirty = true;
        updateBounds(_bounds + Vec2f(d.x(), d.y()));
    }
}

float DisplayObject::x() const {
    return position().x();
}

void DisplayObject::x(float value) {
    auto pos = _matrix->translation();
    if (pos.x() != value) {
        float d = value - pos.x();
        pos.x() = value;
        _matrixDirty = true;
        updateBounds(_bounds + Vec2f(d, 0));
    }
}

float DisplayObject::y() const {
    return position().y();
}

void DisplayObject::y(float value) {
    auto pos = _matrix->translation();
    if (pos.y() != value) {
        float d = value - pos.y();
        pos.y() = value;
        _matrixDirty = true;
        updateBounds(_bounds + Vec2f(0, d));
    }
}

float DisplayObject::z() const {
    return position().z();
}

void DisplayObject::z(float value) {
    auto pos = _matrix->translation();
    if (pos.z() != value) {
        pos.z() = value;
        _matrixDirty = true;
    }
}

Vec3f DisplayObject::rotation() const {
    Vec3f rot;
    math::matrixLinear(_matrix, &rot, nullptr);
    return math::angle(rot);
}

void DisplayObject::rotation(const Vec3f &value) {
    Matrix3f scale;
    math::matrixLinear(_matrix, (Vec3f*)nullptr, &scale);
    _matrix->linear() = math::rotation(math::radian(value)) * scale;
    _matrixDirty = true;
    updateBounds();
}

float DisplayObject::rotationX() const {
    return rotation().x();
}

void DisplayObject::rotationX(float value) {
    Vec3f rot;
    Matrix3f scale;
    math::matrixLinear(_matrix, &rot, &scale);
    rot.x() = math::radian(value);
    _matrix->linear() = math::rotation(rot) * scale;
    _matrixDirty = true;
    updateBounds();
}

float DisplayObject::rotationY() const {
    return rotation().y();
}

void DisplayObject::rotationY(float value) {
    Vec3f rot;
    Matrix3f scale;
    math::matrixLinear(_matrix, &rot, &scale);
    rot.y() = math::radian(value);
    _matrix->linear() = math::rotation(rot) * scale;
    _matrixDirty = true;
    updateBounds();
}

float DisplayObject::rotationZ() const {
    return rotation().z();
}

void DisplayObject::rotationZ(float value) {
    Vec3f rot;
    Matrix3f scale;
    math::matrixLinear(_matrix, &rot, &scale);
    rot.z() = math::radian(value);
    _matrix->linear() = math::rotation(rot) * scale;
    _matrixDirty = true;
    updateBounds();
}

Vec3f DisplayObject::scale() const {
    Matrix3f m;
    math::matrixLinear(_matrix, (Matrix3f*)nullptr, &m);
    return Vec3f(m(0, 0), m(1, 1), m(2, 2));
}

void DisplayObject::scale(const Vec3f &value) {
    Matrix3f rot;
    math::matrixLinear(_matrix, &rot, nullptr);
    _matrix->linear() = rot * Scaling(value);
    _matrixDirty = true;
    updateBounds();
}

float DisplayObject::scaleX() const {
    return scale().x();
}

void DisplayObject::scaleX(float value) {
    Matrix3f rot;
    math::matrixLinear(_matrix, &rot, nullptr);
    _matrix->linear() = rot * Scaling(value, 0.f, 0.f);
    _matrixDirty = true;
    updateBounds();
}

float DisplayObject::scaleY() const {
    return scale().y();
}

void DisplayObject::scaleY(float value) {
    Matrix3f rot;
    math::matrixLinear(_matrix, &rot, nullptr);
    _matrix->linear() = rot * Scaling(0.f, value, 0.f);
    _matrixDirty = true;
    updateBounds();
}

float DisplayObject::scaleZ() const {
    return scale().z();
}

void DisplayObject::scaleZ(float value) {
    Matrix3f rot;
    math::matrixLinear(_matrix, &rot, nullptr);
    _matrix->linear() = rot * Scaling(0.f, 0.f, value);
    _matrixDirty = true;
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
    updateBounds((*_matrix) * contentBounds());
}

const Matrix &DisplayObject::concatenatedMatrix() noexcept {
    size_t old_size = __objects.size();
    DisplayObject *parent = _parent; 
    size_t n = -1;
    while (parent) {
        __objects.emplace_back(parent);
        if (parent->_matrixDirty) {
            n = __objects.size();
        }
        parent = parent->_parent; 
    }
    Matrix *trans;
    if (n != (size_t)-1) {
        DisplayObject *obj = __objects[n];
        if (obj->_parent) {
            *obj->_concatenatedMatrix = (*obj->_parent->_concatenatedMatrix) * (*obj->_matrix);
            trans = obj->_concatenatedMatrix;
        }
        else {
            trans = obj->_matrix;
        }
        for (--n; n <= old_size; --n) {
            obj = __objects[n];
            *obj->_concatenatedMatrix = (*trans) * (*obj->_matrix);
            trans = obj->_concatenatedMatrix;
        }
    }
    else {
        if (!_parent) {
            trans = _matrix;
            goto finish;
        }
        if (!_matrixDirty) {
            trans = _concatenatedMatrix;
            goto finish;
        }
        trans = _parent->_concatenatedMatrix; 
    }

    *_concatenatedMatrix = (*trans) * (*_matrix);
    trans = _concatenatedMatrix;
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
            return targetCoordinateSpace->concatenatedMatrix().inverse() * 
                (_parent->concatenatedMatrix() * _bounds);
        }
        else {
            return targetCoordinateSpace->concatenatedMatrix().inverse() * _bounds;
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

void DisplayObject::stage(Stage *stage) {
    _stage = stage;
    if (stage) {
        EventDispatcher::dispatchEvent(object<Event>(Event::ADD_TO_STAGE));
    }
    else {
        EventDispatcher::dispatchEvent(object<Event>(Event::REMOVED_FROM_STAGE));
    }
}

void DisplayObject::render(Renderer &renderer, const Matrix &mat, int dirty) noexcept {
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
}

GV_NS_END


