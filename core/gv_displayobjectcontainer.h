#ifndef __GV_DISPLAY_OBJECT_CONTAINER_H__
#define __GV_DISPLAY_OBJECT_CONTAINER_H__

#include "gv_interactiveobject.h"

GV_NS_BEGIN

class DisplayObjectContainer : public InteractiveObject {
    friend class Object;
    friend class DisplayObject;
    friend class Stage;
public:
    typedef gv_list(ptr<DisplayObject>, _entry) ContainerBase;
    class Container : private ContainerBase {
        friend class DisplayObjectContainer;
    public:
        using ContainerBase::first;
        using ContainerBase::front;
        using ContainerBase::last;
        using ContainerBase::back;
        using ContainerBase::next;
        using ContainerBase::prev;
        using ContainerBase::empty;
        using ContainerBase::iterator;
        using ContainerBase::reverse_iterator;
        using ContainerBase::begin;
        using ContainerBase::end;
        using ContainerBase::rbegin;
        using ContainerBase::rend;
        using ContainerBase::cbegin;
        using ContainerBase::cend;
        using ContainerBase::crbegin;
        using ContainerBase::crend;
        Container& operator=(const Container&) = delete;
    protected:
        Container() noexcept : _size() {}
    private:
        unsigned _size;
    };

    virtual DisplayObject *addChild(const ptr<DisplayObject> &child);
    virtual DisplayObject *addChild(const ptr<DisplayObject> &child, unsigned index);
    virtual DisplayObject *addChild(const ptr<DisplayObject> &child, DisplayObject *before);

    virtual ptr<DisplayObject> removeChild(const ptr<DisplayObject> &child);
    virtual ptr<DisplayObject> removeChild(unsigned index);
    virtual void removeChildren();

    unsigned getChildIndex(DisplayObject *child);
    virtual void setChildIndex(DisplayObject *child, unsigned index);

    DisplayObject *getChildAt(unsigned index);
    DisplayObject *getChildByName(UniStr *name, bool recursive = false);
    bool contains(DisplayObject *child);

    virtual void swapChildren(const ptr<DisplayObject> &a, const ptr<DisplayObject> &b);
    virtual void swapChildren(unsigned a, unsigned b);

    unsigned numChildren() const noexcept {
        return _container._size;
    }

    virtual void bringChildToFront(const ptr<DisplayObject> &child);
    virtual void sendChildToBack(const ptr<DisplayObject> &child);

protected:
    DisplayObjectContainer() noexcept;
    virtual void updateChildBounds(DisplayObject *child, const Box2f &oldBounds, const Box2f &newBounds);
    virtual Box2f contentBounds() override;

private:
    ptr<DisplayObject> removeChild(const ptr<DisplayObject> &child, bool update);
    virtual void stage(Stage *stage) override;
    void render(Renderer &renderer, const Matrix &mat, int dirty) noexcept;

private:
    Box2f _childrenBounds;
    Container _container;
};

GV_NS_END

#endif
