#ifndef __GV_ENV_H__
#define __GV_ENV_H__

#include "gv_object.h"
#include "gv_singleton.h"
#include "gv_pixel.h"
#include "gv_math.h"

GV_NS_BEGIN

class Env : public Object, public singleton<Env> {
    GV_FRIEND_SINGLETON();
public:
    int maxTextureSize() const noexcept {
          return _maxTextureSize;
}
    float glVersion() const noexcept {
        return _glVersion;
    }
    PixelFormat defaultPixelFormat;
    unsigned stageWidth;
    unsigned stageHeight;
    std::string title;
private:
    Env();
    float _glVersion;
    int _maxTextureSize;
};

GV_NS_END

#endif
