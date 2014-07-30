#ifndef __GV_SHAPE_H__
#define __GV_SHAPE_H__

#include "gv_displayobject.h"
#include "gv_graphics.h"

GV_NS_BEGIN

class Shape : public DisplayObject {
    friend class Object;
public:
    Graphics &graphics() noexcept {
        return *_graphics;
    }

protected:
    Shape() noexcept { }
    virtual void draw(Renderer &renderer, const Matrix &mat) override;
private:
    object<Graphics> _graphics;
};


GV_NS_END


#endif

