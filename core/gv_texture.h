#ifndef __GV_TEXTURE_H__
#define __GV_TEXTURE_H__

#include "gv_object.h"
#include "gv_pixel.h"
#include "gv_path.h"
#include "gv_image.h"

GV_NS_BEGIN

class Texture : public Object {
public:
    static ptr<Texture> create(Chunk *chunk, unsigned width, unsigned height, PixelInfo *info, size_t count = 1) noexcept;
    static ptr<Texture> create(Image *image, PixelFormat format) noexcept;
    static ptr<Texture> create(const ptr<Path> &path, PixelFormat format = PixelFormat::UNKNOWN) noexcept;

    Texture();

protected:
    ~Texture();
private:
    PixelInfo *_pixelInfo;
    GLuint _id;
    bool _antialias;
    unsigned _width;
    unsigned _height;
};

GV_NS_END

#endif
