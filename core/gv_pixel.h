#ifndef __GV_PIXEL_H__
#define __GV_PIXEL_H__

#include "gv_object.h"
#include "gv_log.h"

GV_NS_BEGIN

class Chunk;

enum class PixelFormat {
    A8,
    I8,
    AI88,
    RGB888,
    RGBA8888,
    RGB565,
    RGBA4444,
    RGBA5551,
    UNKNOWN,
};

struct PixelInfo : Object {
    virtual ptr<Chunk> convert(const Chunk &src, PixelFormat to) const noexcept;

    virtual bool support() const noexcept {
        return true;
    }
    const char *desc() const noexcept {
        return _desc;
    }
    PixelFormat format() const noexcept {
        return _format;
    }
    bool compressed() const noexcept {
        return _compressed;
    }
    bool alpha() const noexcept {
        return _alpha;
    }
    size_t pixelSize() const noexcept {
        return _pixelSize;
    }
    GLint glInternalFormat() const noexcept {
        return _glInternalFormat;
    }
    GLenum glFormat() const noexcept {
        return _glFormat;
    }
    GLenum glType() const noexcept {
        return _glType;
    }
    static ptr<PixelInfo> get(PixelFormat fmt) noexcept {
        gv_assert(fmt < PixelFormat::UNKNOWN, "unknown pixel format '%d'.", static_cast<int>(fmt));
        return _infos[static_cast<size_t>(fmt)];
    }

protected:
    PixelInfo(PixelFormat format, const char *desc, bool compressed, bool alpha, size_t pixelSize,
        GLint glInternalFormat, GLenum glFormat, GLenum glType);

    static ptr<PixelInfo> _infos[static_cast<size_t>(PixelFormat::UNKNOWN)];
    PixelFormat _format;
    const char *_desc;
    bool _compressed;
    bool _alpha;
    size_t _pixelSize;
    GLint _glInternalFormat;
    GLenum _glFormat;
    GLenum _glType;
};

struct PixelFormatInfo {

    PixelFormatInfo(GLenum anInternalFormat, GLenum aFormat, GLenum aType, int aBpp, bool aCompressed, bool anAlpha)
    : internalFormat(anInternalFormat)
    , format(aFormat)
    , type(aType)
    , bpp(aBpp)
    , compressed(aCompressed)
    , alpha(anAlpha)
    {}

    GLenum internalFormat;
    GLenum format;
    GLenum type;
    int bpp;
    bool compressed;
    bool alpha;
};

GV_NS_END

#endif
