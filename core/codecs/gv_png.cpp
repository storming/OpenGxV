#include "opengxv.h"
#include "gv_png.h"
#include "gv_pixel.h"
#include "gv_chunk.h"

GV_NS_BEGIN

PngCodec::decoder::decoder(File *file) :
_file(file),
_png(),
_info(),
_endinfo() {}

PngCodec::decoder::~decoder() noexcept {
    if (_png) {
        png_destroy_read_struct(&_png, _info ? &_info : nullptr, _endinfo ? &_endinfo : nullptr);
    }
}

void PngCodec::decoder::callback(png_structp png, png_bytep data, png_size_t size) noexcept {
    decoder* self = (decoder*)png_get_io_ptr(png);

    if (self->_file->read(data, size) != size) {
        png_error(png, "read callback failed");
    }
}

ptr<Image> PngCodec::decoder::load() noexcept {
    if (!(_png = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0))) {
        return nullptr;
    }
    if (!(_info = png_create_info_struct(_png))) {
        return nullptr;
    }
    if (!(_endinfo = png_create_info_struct(_png))) {
        return nullptr;
    }
    if (setjmp(png_jmpbuf(_png))) {
        return nullptr;
    }

    size_t rowbytes;
    int depth;
    int color;

    object<Image> image;
    png_set_read_fn(_png, this, callback);
    png_read_info(_png, _info);
    png_get_IHDR(_png, _info, &image->_width, &image->_height, &depth, &color, 0, 0, 0);

    if (color == PNG_COLOR_TYPE_PALETTE) {
        png_set_palette_to_rgb(_png);
    }
    // low-bit-depth grayscale images are to be expanded to 8 bits
    if (color == PNG_COLOR_TYPE_GRAY && depth < 8) {
        depth = 8;
        png_set_expand_gray_1_2_4_to_8(_png);
    }
    // expand any tRNS chunk data into a full alpha channel
    if (png_get_valid(_png, _info, PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(_png);
    }
    // reduce images with 16-bit samples to 8 bits
    if (depth == 16) {
        png_set_strip_16(_png);
    }

    // Expanded earlier for grayscale, now take care of palette and rgb
    if (depth < 8) {
        png_set_packing(_png);
    }
    // update info
    png_read_update_info(_png, _info);
    depth = png_get_bit_depth(_png, _info);
    color = png_get_color_type(_png, _info);

    switch (color) {
    case PNG_COLOR_TYPE_GRAY:
        image->_pixelInfo = PixelInfo::get(PixelFormat::I8);
        break;
    case PNG_COLOR_TYPE_GRAY_ALPHA:
        image->_pixelInfo = PixelInfo::get(PixelFormat::AI88);
        break;
    case PNG_COLOR_TYPE_RGB:
        image->_pixelInfo = PixelInfo::get(PixelFormat::RGB888);
        break;
    case PNG_COLOR_TYPE_RGB_ALPHA:
        image->_pixelInfo = PixelInfo::get(PixelFormat::RGBA8888);
        break;
    default:
        return nullptr;
    }
    rowbytes = png_get_rowbytes(_png, _info);
    size_t size = rowbytes * image->_height;
    Chunk chunk(size);

    if (!chunk.data()) {
        return nullptr;
    }
    Chunk tmp(sizeof(unsigned char*)* image->_height);
    unsigned char **rows = (unsigned char**)tmp.data();
    if (!rows) {
        return nullptr;
    }
    for (unsigned i = 0; i < image->_height; i++) {
        rows[i] = chunk.data() + i * rowbytes;
    }
    png_read_image(_png, rows);
    png_read_end(_png, _endinfo);

    image->_mipmaps.emplace_back(std::move(chunk));
    return image;
}

ptr<Image> PngCodec::load(File *file) noexcept {
    return decoder(file).load();
}

GV_NS_END
