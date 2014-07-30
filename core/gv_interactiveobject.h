#ifndef __GV_INTERACTIVE_OBJECT_H__
#define __GV_INTERACTIVE_OBJECT_H__

#include "gv_displayobject.h"

GV_NS_BEGIN

class InteractiveObject : public DisplayObject {
    friend class DisplayObjectContainer;
protected:
    InteractiveObject() noexcept : DisplayObject() {}
private:
    InteractiveObject(bool iscontainer) noexcept : DisplayObject(iscontainer) {}
};

GV_NS_END


#endif

