#ifndef __GV_PNG_H__
#define __GV_PNG_H__

#include "gv_image.h"
#include "gv_file.h"

extern "C" {
#include "png.h"
}

GV_NS_BEGIN

class PngCodec final {
public:
    static ptr<Image> load(File *file) noexcept;
private:
    struct decoder {
        File *_file;
        png_structp _png;
        png_infop _info;
        png_infop _endinfo;
        decoder(File *file);
        ~decoder();
        static void callback(png_structp png, png_bytep data, png_size_t size) noexcept;
        ptr<Image> load() noexcept;
    };
};

GV_NS_END

#endif
