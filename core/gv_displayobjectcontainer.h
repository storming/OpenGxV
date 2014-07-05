#ifndef __GV_DISPLAY_OBJECT_CONTAINER_H__
#define __GV_DISPLAY_OBJECT_CONTAINER_H__

#include "gv_displayobject.h"

GV_NS_BEGIN

class DisplayObjectContainer : public DisplayObject {
    GV_FRIEND_PTR();
    friend DisplayObject;
private:
    typedef gv_list(ptr<DisplayObject>, _entry) container_base;

public:
    class ContainerType : protected container_base {
        friend class DisplayObjectContainer;
        friend class DisplayObject;
    protected:
        ContainerType() noexcept : container_base() {}
        ContainerType(const ContainerType&) = delete;
        ContainerType(ContainerType &&x) noexcept : container_base(std::move(x)) {}
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
    };

    virtual DisplayObject *addChild(ptr<DisplayObject> child);
    virtual DisplayObject *addChild(ptr<DisplayObject> child, DisplayObject *before);
    virtual void swapChild(DisplayObject *a, DisplayObject *b);
    virtual DisplayObject *child(size_t index);
    const ContainerType &children() noexcept {
        return _list;
    }
    virtual ptr<DisplayObject> removeChild(DisplayObject *child);
    virtual void removeChildren();
protected:
    DisplayObjectContainer() noexcept {}
    virtual void updateBounds(DisplayObject *child, Box2f *newBounds);
    virtual void updateBounds() override;
private:
    ContainerType _list;
};

GV_NS_END

#endif
