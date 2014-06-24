#include "opengv.h"
#include "gv_image.h"
#include "gv_file.h"
#include "codecs/gv_png.h"

GV_NS_BEGIN

Image::Image() noexcept :
_pixelInfo(),
_pmAlpha(false),
_width(),
_height()
{}

ptr<Image> Image::load(const ptr<Path> &path, FileType type) noexcept {
    if (FileType::UNKNOWN == type) {
        type = File::type(path);
        if (FileType::UNKNOWN == type) {
            return nullptr;
        }
    }

    ptr<File> file = File::load(path);
    if (!file) {
        return nullptr;
    }

    ptr<Image> image;
    switch (type) {
    case FileType::PNG:
        image = PngCodec::load(file);
        break;
    }
    return image;
}

GV_NS_END

