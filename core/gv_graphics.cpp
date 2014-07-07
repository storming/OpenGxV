#include "opengv.h"
#include "gv_graphics.h"

GV_NS_BEGIN


void Graphics::clear() noexcept {
    base::clear();
    _bounds.setZero();
}

void Graphics::drawPrimitive(const ptr<Primitive> &primitive) noexcept {
    if (empty()) {
        _bounds = primitive->bounds();
    }
    else {
        _bounds |= primitive->bounds();
    }
    emplace_back(primitive);
}


GV_NS_END