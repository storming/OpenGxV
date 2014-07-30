#ifndef __GV_PRIMITIVE_H__
#define __GV_PRIMITIVE_H__

#include "gv_object.h"
#include "gv_math.h"

GV_NS_BEGIN

struct Primitive : public Object {
    virtual Box2f bounds() = 0;
};

class PrimitiveLines : public Primitive {
public:
    void moveTo(float x, float y);
    void lineTo(float x, float y);
private:
    Vec3f _pos;
};

GV_NS_END

#endif