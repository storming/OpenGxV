#ifndef __GV_PRIMITIVE_H__
#define __GV_PRIMITIVE_H__

#include "gv_object.h"
#include "gv_math.h"

GV_NS_BEGIN

struct Primitive : public Object {
    virtual Box2f bounds() = 0;
};

GV_NS_END

#endif