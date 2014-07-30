#ifndef __GV_DISPLAY_OBJECT_H__
#define __GV_DISPLAY_OBJECT_H__

#include "gv_object.h"
#include "gv_math.h"
#include "gv_graphics.h"
#include "gv_log.h"
#include "gv_eventdispatcher.h"
#include "gv_renderer.h"

GV_NS_BEGIN

class DisplayObjectContainer;
class Stage;

class DisplayObject : public EventDispatcher {
    friend class Object;
    friend class DisplayObjectContainer;
    friend class InteractiveObject;
    friend class Stage;
    GV_FRIEND_LIST();
public:
    virtual Size2f size() const;
    virtual void size(const Size2f &value);

    virtual float width() const;
    virtual void width(float value);

    virtual float height() const;
    virtual void height(float value);

    virtual Vec3f position() const;
    virtual void position(const Vec3f &value);

    virtual float x() const;
    virtual void x(float value);

    virtual float y() const;
    virtual void y(float value);

    virtual float z() const;
    virtual void z(float value);

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

    virtual bool visible() const;
    virtual void visible(bool value);

    const ptr<UniStr> &name() const noexcept {
        return _name;
    }

    void name(const ptr<UniStr> &value) noexcept {
        _name = value;
    }

    void name(const char *value) noexcept {
        _name = unistr(value);
    }

    void name(const std::string &value) noexcept {
        _name = unistr(value);
    }

    DisplayObjectContainer *parent() const noexcept {
        return _parent;
    }

    Stage *stage() const noexcept {
        return _stage;
    }

    Matrix &matrix() noexcept {
        return *_matrix;
    }

    void matrix(const Matrix &mat) noexcept {
        const Matrix *m = std::addressof<const Matrix>(mat);
        if (!m){
            return;
        }
        if (m != _matrix){
            *_matrix = mat;
        }
        _matrixDirty = true;
        updateBounds();
    }

    const Matrix &concatenatedMatrix() noexcept;
    virtual Box2f bounds(DisplayObject *targetCoordinateSpace);
    virtual bool dispatchEvent(ptr<Event> event) override;

protected:
    DisplayObject() noexcept : DisplayObject(false) { }
    virtual Box2f contentBounds() = 0;
    virtual void updateBounds();
    virtual void draw(Renderer &renderer, const Matrix &mat) = 0;

private:
    DisplayObject(bool iscontainer) noexcept;
    void updateBounds(const Box2f &bounds) noexcept;
    bool dispatchEvent(DisplayObject *parent, ptr<Event> event) noexcept;
    virtual void stage(Stage *stage);
    void render(Renderer &renderer, const Matrix &mat, int dirty) noexcept;

private:
    clist_entry             _entry;
    DisplayObjectContainer *_parent;
    Stage                  *_stage;
    ptr<UniStr>             _name;
    owned_ptr<Matrix>       _matrix;
    owned_ptr<Matrix>       _concatenatedMatrix;
    Box2f                   _bounds;
    bool                    _iscontainer;
    bool                    _visible;
    bool                    _matrixDirty;
}; 

GV_NS_END

#endif

