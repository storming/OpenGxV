#ifndef __GV_IMAGE_H__
#define __GV_IMAGE_H__

#include "gv_object.h"
#include "gv_path.h"
#include "gv_file.h"
#include "gv_pixel.h"

GV_NS_BEGIN

class Image : public Object {
    GV_FRIEND_PTR();
    friend class PngCodec;
public:
    static ptr<Image> load(const ptr<Path> &path, FileType type = FileType::UNKNOWN) noexcept;

    bool a(int, int) {

    }
    unsigned width() const noexcept {
        return _width;
    }
    unsigned height() const noexcept {
        return _height;
    }
    bool pmAlpha() const noexcept {
        return _pmAlpha;
    }
    PixelInfo *pixelInfo() const noexcept {
        return _pixelInfo;
    }
    std::vector<Chunk> &mipmaps() noexcept {
        return _mipmaps;
    }
protected:
    Image() noexcept;
    PixelInfo *_pixelInfo;
    bool _pmAlpha;
    unsigned _width;
    unsigned _height;
    std::vector<Chunk> _mipmaps;
};

GV_NS_END

#endif