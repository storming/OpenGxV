#ifndef __GV_DISPLAY_OBJECT_H__
#define __GV_DISPLAY_OBJECT_H__

#include "gv_object.h"
#include "gv_math.h"
#include "gv_graphics.h"
#include "gv_log.h"
#include "gv_list.h"
#include "gv_eventdispatcher.h"
GV_NS_BEGIN

class DisplayObjectContainer;

class DisplayObject : public EventDispatcher {
    GV_FRIEND_PTR();
    friend class DisplayObjectContainer;
public:
    Size2f size() const noexcept {
          return _bounds.size();
    }
    virtual void size(const Size2f &value);

    float width() const noexcept {
        return _bounds.width();
    }
    virtual void width(float value);

    float height() const noexcept {
        return _bounds.height();
    }
    virtual void height(float value);

    const Box2f &bounds() {
        if (_flags & BOUNDS_DIRTY) {
            updateBounds();
        }
        return _bounds;
    }

    Transform::TranslationPart position() const {
        return _transform.translation();
    }
    void position(const Vec3f &value);
    float x() const {
        return _transform.translation().x();
    }
    void x(float value);
    float y() const noexcept {
        return _transform.translation().y();
    }
    void y(float value);
    float z() const noexcept {
        return _transform.translation().z();
    }
    void z(float value);

    virtual Vec3f rotation() const;
    virtual void rotation(const Vec3f &value);
    virtual float rotationX() const;
    virtual void rotationX(float value);
    virtual float rotationY() const;
    virtual void rotationY(float value);
    virtual float rotationZ() const;
    virtual void rotationZ(float value);

    virtual Vec3f scale() const;
    virtual void scale(const Vec3f &value);
    virtual float scaleX() const;
    virtual void scaleX(float value);
    virtual float scaleY() const;
    virtual void scaleY(float value);
    virtual float scaleZ() const;
    virtual void scaleZ(float value);

    DisplayObjectContainer *parent() const noexcept {
        return static_cast<DisplayObjectContainer*>(_parent);
    }
    Transform &transform() noexcept {
        return _transform;
    }
    void transform(const Transform &t) noexcept {
        if (std::addressof<const Transform>(t) != &_transform) {
            _transform = t;
        }
        _flags |= (BOUNDS_DIRTY | TRANSFORM_DIRTY);
    }
    const Transform &globalTransform() noexcept {
        if (_flags & TRANSFORM_DIRTY && _parent) {
            _globalTransform = reinterpret_cast<DisplayObject*>(_parent)->globalTransform() * _transform;
            _flags &= (~TRANSFORM_DIRTY);
        }
        return _globalTransform;
    }
    virtual bool visible() {
        return (_flags & VISIBLE_FLAG) != 0;
    }
    virtual void visible(bool value) {
        if (value) {
            _flags |= VISIBLE_FLAG;
        }
        else {
            _flags &= (~VISIBLE_FLAG);
        }
    }
    void bringToFront() noexcept;
    void sendToBack() noexcept;
    //virtual bool draw()
protected:
    DisplayObject();

    virtual void updateBounds();
    virtual Box2f contentBounds() = 0;

    void linear(Vec3f *rot, Matrix3f *scale) const;
    void linear(Matrix3f *rot, Matrix3f *scale) const {
        _transform.computeRotationScaling(rot, scale);
    }

private:
    bool attach(DisplayObjectContainer *container) noexcept;
    void dettach(DisplayObjectContainer *container) noexcept;

private:
    clist_entry _entry;

protected:
    enum {
        VISIBLE_FLAG            = 1 << 0,
        OOV_FLAG                = 1 << 1,
        TRANSFORM_DIRTY         = 1 << 2,
        BOUNDS_DIRTY            = 1 << 3,
    };
    Box2f _bounds;
    mutable Transform _transform;
    Transform _globalTransform;
    unsigned _flags;
};

GV_NS_END

#endif
