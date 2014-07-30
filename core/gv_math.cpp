#include "opengxv.h"
#include "gv_math.h"

GV_NS_BEGIN
GV_MATH_BEGIN

void ortho(float left, float right, float top, float bottom, float n, float f, Matrix &mat) noexcept {
    mat.setIdentity();
    mat.scale(Vec3f(
        2.0f / (right - left), 
        2.0f / (top - bottom), 
        2.0f / (n - f)
    ));
    mat.translate(Vec3f(
        (right + left) / (left - right),
        (top + bottom) / (bottom - top),
        (f + n) / (n - f)
    ));
}

GV_MATH_END
GV_NS_END