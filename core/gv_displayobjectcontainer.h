#ifndef __GV_DISPLAY_OBJECT_CONTAINER_H__
#define __GV_DISPLAY_OBJECT_CONTAINER_H__

#include "gv_displayobject.h"

GV_NS_BEGIN

class DisplayObjectContainer : public DisplayObject {
    friend class Object;
    friend class DisplayObject;
private:
    typedef gv_list(ptr<DisplayObject>, _entry) container_base;

public:
    class ContainerType : protected container_base {
        friend class DisplayObjectContainer;
    public:
        using container_base::swap;
        using container_base::operator=;
        using container_base::first;
        using container_base::front;
        using container_base::last;
        using container_base::back;
        using container_base::next;
        using container_base::prev;
        using container_base::empty;
        using container_base::iterator;
        using container_base::reverse_iterator;
        using container_base::begin;
        using container_base::end;
        using container_base::rbegin;
        using container_base::rend;
        using container_base::cbegin;
        using container_base::cend;
        using container_base::crbegin;
        using container_base::crend;
        unsigned size() const noexcept {
            return _size;
        }
    protected:
        ContainerType() noexcept : container_base() {}
        ContainerType(const ContainerType&) = delete;
        ContainerType(ContainerType &&x) noexcept : container_base(std::move(x)) {}
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

    virtual void bringChildToFront(const ptr<DisplayObject> &child);
    virtual void sendChildToBack(const ptr<DisplayObject> &child);

protected:
    DisplayObjectContainer() noexcept;
    virtual void updateChildBounds(DisplayObject *child, const Box2f &oldBounds, const Box2f &newBounds);
    virtual Box2f contentBounds() override;

private:
    ptr<DisplayObject> removeChild(const ptr<DisplayObject> &child, bool update);

private:
    Box2f _childrenBounds;
    ContainerType _list;
};

GV_NS_END

#endif
