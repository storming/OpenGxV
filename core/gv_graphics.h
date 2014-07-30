#ifndef __GV_GRAPHICS_H__
#define __GV_GRAPHICS_H__

#include <vector>
#include "gv_object.h"
#include "gv_primitive.h"

GV_NS_BEGIN

class GraphicsData : public Object {
};

class Graphics : public Object {
public:
    void clear() noexcept;
    void drawPrimitive(const ptr<Primitive> &primitive) noexcept;
private:
    typedef std::vector<ptr<Primitive>> base;
    Box2f _bounds;
};

GV_NS_END

#endif
