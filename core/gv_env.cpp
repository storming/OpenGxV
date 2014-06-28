#include "opengv.h"
#include "gv_env.h"

GV_NS_BEGIN

Env::Env() : 
    defaultPixelFormat(PixelFormat::RGBA8888) {
    //_glVersion = atof((const char*)glGetString(GL_VERSION));
    //glGetIntegerv(GL_MAX_TEXTURE_SIZE, &_maxTextureSize);
}


GV_NS_END