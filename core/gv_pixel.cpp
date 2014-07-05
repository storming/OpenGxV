#include "opengv.h"
#include "gv_pixel.h"
#include "gv_chunk.h"

GV_NS_BEGIN

PixelInfo::PixelInfo(PixelFormat format, const char *desc, bool compressed, bool alpha, size_t pixelSize, GLint glInternalFormat, GLenum glFormat, GLenum glType) 
: _format(format),
  _desc(desc),
  _compressed(compressed),
  _alpha(alpha),
  _pixelSize(pixelSize),
  _glInternalFormat(glInternalFormat),
  _glFormat(glFormat),
  _glType(glType) 
{ }

ptr<Chunk> PixelInfo::convert(const Chunk &src, PixelFormat to) const noexcept {
    return nullptr;
}

#define INFO_CONSTRUCTOR(x, z, a, s, ifmt, f, t) PixelInfo##x() : PixelInfo(PixelFormat::x, #x, z, a, s, ifmt, f, t) {}
struct PixelInfoA8 : PixelInfo {
    INFO_CONSTRUCTOR(A8, false, true, 1, GL_ALPHA, GL_ALPHA, GL_UNSIGNED_BYTE);
};

struct PixelInfoI8 : PixelInfo {
    INFO_CONSTRUCTOR(I8, false, false, 1, GL_LUMINANCE, GL_LUMINANCE, GL_UNSIGNED_BYTE);

    virtual ptr<Chunk> convert(const Chunk &src, PixelFormat to) const noexcept override {
        size_t size = src.size();
        unsigned char *s = src.data();
        unsigned char *end = s + src.size();
        ptr<Chunk> dst;
        switch (to) {
        case PixelFormat::AI88:
            do {
                dst = object<Chunk>(size * 2);
                unsigned short *d = (unsigned short*)dst->data();
                while (s < end) {
                    *d++ = 0xff00 | *s++;
                }
            } while (0);
            break;
        case PixelFormat::RGB888: 
            do {
                dst = object<Chunk>(size * 3);
                unsigned char *d = dst->data();
                while (s < end) {
                    *d++ = *s;
                    *d++ = *s;
                    *d++ = *s++;
                }
            } while (0);
            break;
        case PixelFormat::RGBA8888:
            do {
	            dst = object<Chunk>(size * 4);
	            unsigned char *d = dst->data();
	            while (s < end) {
	                *d++ = *s;
	                *d++ = *s;
	                *d++ = *s++;
	                *d++ = 0xff;
	            }
            } while (0);
            break;
        case PixelFormat::RGB565:
            do {
                dst = object<Chunk>(size * 2);
                unsigned short *d = (unsigned short*)dst->data();
                unsigned int c;
                while (s < end) {
                    c = *s++;
                    *d++ = ((c & 0xf8) << 8) | ((c & 0xfc) << 3) | ((c & 0xf8) >> 3);
                }

            } while (0);
            break;
        case PixelFormat::RGBA4444:
            do {
                dst = object<Chunk>(size * 2);
                unsigned short *d = (unsigned short*)dst->data();
                unsigned int c;
                while (s < end) {
                    c = *s++;
                    *d++ = ((c & 0xf0) << 8) | ((c & 0xf0) << 4) | (c & 0xf0) | 0xf;
                }
            } while (0);
            break;
        case PixelFormat::RGBA5551:
            do {
                dst = object<Chunk>(size * 2);
                unsigned short *d = (unsigned short*)dst->data();
                unsigned int c;
                while (s < end) {
                    c = *s++;
                    *d++ = ((c & 0xf8) << 8) | ((c & 0xf8) << 3) | ((c & 0xf8) >> 2) | 1;
                }
            } while (0);
            break;
        }
        return dst;
    }
};

struct PixelInfoAI88 : PixelInfo {
    INFO_CONSTRUCTOR(AI88, false, true, 2, GL_LUMINANCE_ALPHA, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE);

    virtual ptr<Chunk> convert(const Chunk &src, PixelFormat to) const noexcept override {
        size_t size = src.size();
        unsigned char *s = src.data();
        unsigned char *end = s + src.size();
        ptr<Chunk> dst;
        size >>= 1;
        switch (to) {
        case PixelFormat::A8:
            do {
                dst = object<Chunk>(size);
                unsigned char *d = dst->data();
                s++;
                while (s < end) {
                    *d++ = *s;
                    s += 2;
                }
            } while (0);
            break;
        case PixelFormat::I8:
            do {
                dst = object<Chunk>(size);
                unsigned char *d = dst->data();
                while (s < end) {
                    *d++ = *s;
                    s += 2;
                }
            } while (0);
            break;
        case PixelFormat::RGB888:
            do {
                dst = object<Chunk>(size * 3);
                unsigned char *d = dst->data();
                while (s < end) {
                    *d++ = *s;
                    *d++ = *s;
                    *d++ = *s++;
                }
            } while (0);
            break;
        case PixelFormat::RGBA8888:
            do {
                dst = object<Chunk>(size * 4);
                unsigned char *d = dst->data();
                while (s < end) {
                    *d++ = *s;
                    *d++ = *s;
                    *d++ = *s++;
                    *d++ = *s++;
                }
            } while (0);
            break;
        case PixelFormat::RGB565:
            do {
                dst = object<Chunk>(size * 2);
                unsigned short *d = (unsigned short*)dst->data();
                unsigned int c;
                while (s < end) {
                    c = *s;
                    *d++ = ((c & 0xf8) << 8) | ((c & 0xfc) << 3) | ((c & 0xf8) >> 3);
                    s += 2;
                }
            } while (0);
            break;
        case PixelFormat::RGBA4444:
            do {
                dst = object<Chunk>(size * 2);
                unsigned short *d = (unsigned short*)dst->data();
                unsigned int c;
                while (s < end) {
                    c = *s++;
                    *d++ = ((c & 0xf0) << 8) | ((c & 0xf0) << 4) | ((c & 0xf0)) | ((*s++ & 0xf0) >> 4);
                }
            } while (0);
            break;
        case PixelFormat::RGBA5551:
            do {
                dst = object<Chunk>(size * 2);
                unsigned short *d = (unsigned short*)dst->data();
                unsigned int c;
                while (s < end) {
                    c = *s++;
                    *d++ = ((c & 0xf8) << 8) | ((c & 0xf8) << 3) | ((c & 0xf8) >> 2) | ((*s++ & 0x80) >> 7);
                }
            } while (0);
            break;
        }
        return dst;
    }
};

struct PixelInfoRGB888 : PixelInfo {
    INFO_CONSTRUCTOR(RGB888, false, false, 3, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);

    virtual ptr<Chunk> convert(const Chunk &src, PixelFormat to) const noexcept override {
        size_t size = src.size();
        unsigned char *s = src.data();
        unsigned char *end = s + src.size();
        ptr<Chunk> dst;
        size /= 3;
        switch (to) {
        case PixelFormat::I8:
            do {
                dst = object<Chunk>(size);
                unsigned char *d = dst->data();
                while (s < end) {
                    *d++ = (*s++ * 299 + *s++ * 587 + *s++ * 114 + 500) / 1000;
                }
            } while (0);
            break;
        case PixelFormat::AI88:
            do {
                dst = object<Chunk>(size * 2);
                unsigned short *d = (unsigned short*)dst->data();
                while (s < end) {
                    *d++ = 0xff00 | ((*s++ * 299 + *s++ * 587 + *s++ * 114 + 500) / 1000);
                }
            } while (0);
            break;
        case PixelFormat::RGBA8888:
            do {
                dst = object<Chunk>(size * 4);
                unsigned char *d = dst->data();
                while (s < end) {
                    *d++ = *s++;
                    *d++ = *s++;
                    *d++ = *s++;
                    *d++ = 0xFF;
                }
            } while (0);
            break;
        case PixelFormat::RGB565:
            do {
                dst = object<Chunk>(size * 2);
                unsigned short *d = (unsigned short*)dst->data();
                while (s < end) {
                    *d++ = ((*s++ & 0xf8) << 8) | ((*s++ & 0xfc) << 3) | ((*s++ & 0xf8) >> 3);
                }
            } while (0);
            break;
        case PixelFormat::RGBA4444:
            do {
                dst = object<Chunk>(size * 2);
                unsigned short *d = (unsigned short*)dst->data();
                while (s < end) {
                    *d++ = ((*s++ & 0xf0) << 8) | ((*s++ & 0xf0) << 4) | (*s++ & 0xf0) | 0xf;
                }
            } while (0);
            break;
        case PixelFormat::RGBA5551:
            do {
                dst = object<Chunk>(size * 2);
                unsigned short *d = (unsigned short*)dst->data();
                while (s < end) {
                    *d++ = ((*s++ & 0xf8) << 8) | ((*s++ & 0xf8) << 3) | ((*s++ & 0xf8) >> 2) | 1;
                }
            } while (0);
            break;
        }
        return dst;
    }
};

struct PixelInfoRGBA8888 : PixelInfo {
    INFO_CONSTRUCTOR(RGBA8888, false, true, 4, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);

    virtual ptr<Chunk> convert(const Chunk &src, PixelFormat to) const noexcept override {
        size_t size = src.size();
        unsigned char *s = src.data();
        unsigned char *end = s + src.size();
        ptr<Chunk> dst;
        size >>= 2;
        switch (to) {
        case PixelFormat::A8:
            do {
                dst = object<Chunk>(size);
                unsigned char *d = dst->data();
                s += 3;
                while (s < end) {
                    *d++ = *s;
                    s += 4;
                }
            } while (0);
            break;
        case PixelFormat::I8:
            do {
                dst = object<Chunk>(size);
                unsigned char *d = dst->data();
                while (s < end) {
                    *d++ = (*s++ * 299 + *s++ * 587 + *s * 114 + 500) / 1000;
                    s += 2;
                }
            } while (0);
            break;
        case PixelFormat::AI88:
            do {
                dst = object<Chunk>(size * 2);
                unsigned short *d = (unsigned short*)dst->data();
                while (s < end) {
                    *d++ = ((*s++ * 299 + *s++ * 587 + *s++ * 114 + 500) / 1000);
                    *d++ = *s++;
                }
            } while (0);
            break;
        case PixelFormat::RGB888:
            do {
                dst = object<Chunk>(size * 3);
                unsigned char *d = dst->data();
                while (s < end) {
                    *d++ = *s++;
                    *d++ = *s++;
                    *d++ = *s;
                    s += 2;
                }
            } while (0);
            break;
        case PixelFormat::RGB565:
            do {
                dst = object<Chunk>(size * 2);
                unsigned short *d = (unsigned short*)dst->data();
                while (s < end) {
                    *d++ = ((*s++ & 0xf8) << 8) | ((*s++ & 0xfc) << 3) | ((*s & 0xf8) >> 3);
                    s += 2;
                }
            } while (0);
            break;
        case PixelFormat::RGBA4444:
            do {
                dst = object<Chunk>(size * 2);
                unsigned short *d = (unsigned short*)dst->data();
                while (s < end) {
                    *d++ = ((*s++ & 0xf0) << 8) | ((*s++ & 0xf0) << 4) | (*s++ & 0xf0) | ((*s++ & 0xf0) >> 4);
                }
            } while (0);
            break;
        case PixelFormat::RGBA5551:
            do {
                dst = object<Chunk>(size * 2);
                unsigned short *d = (unsigned short*)dst->data();
                while (s < end) {
                    *d++ = ((*s++ & 0xf8) << 8) | ((*s++ & 0xf8) << 3) | ((*s++ & 0xf8) >> 2) | ((*s++ & 0x80) >> 7);
                }
            } while (0);
            break;
        }
        return dst;
    }
};

struct PixelInfoRGB565 : PixelInfo {
    INFO_CONSTRUCTOR(RGB565, false, false, 2, GL_RGB, GL_RGB, GL_UNSIGNED_SHORT_5_6_5);
};

struct PixelInfoRGBA4444 : PixelInfo {
    INFO_CONSTRUCTOR(RGBA4444, false, true, 2, GL_RGBA, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4);
};

struct PixelInfoRGBA5551 : PixelInfo {
    INFO_CONSTRUCTOR(RGBA5551, false, true, 2, GL_RGBA, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1);
};

ptr<PixelInfo> PixelInfo::_infos[static_cast<size_t>(PixelFormat::UNKNOWN)] = {
    object<PixelInfoA8>(),
    object<PixelInfoI8>(),
    object<PixelInfoAI88>(),
    object<PixelInfoRGB888>(),
    object<PixelInfoRGBA8888>(),
    object<PixelInfoRGB565>(),
    object<PixelInfoRGBA4444>(),
    object<PixelInfoRGBA5551>(),
};

GV_NS_END