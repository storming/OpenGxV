#include "opengv.h"

#include "gv_texture.h"
#include "gv_env.h"

GV_NS_BEGIN

Texture::Texture() : _id(), _antialias(true) {}

Texture::~Texture() {
    if (_id) {
        glDeleteTextures(1, &_id);
    }
}

ptr<Texture> Texture::create(Chunk *chunk, unsigned width, unsigned height, const ptr<PixelInfo> &info, size_t count) noexcept {
    if (!width || !height || count < 1 || !info->support()) {
        return nullptr;
    }

    if (count == 1 && !info->compressed()) {
        unsigned int bytesPerRow = width * info->pixelSize();
        if (bytesPerRow % 8 == 0) {
            glPixelStorei(GL_UNPACK_ALIGNMENT, 8);
        }
        else if (bytesPerRow % 4 == 0) {
            glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        }
        else if (bytesPerRow % 2 == 0) {
            glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
        }
        else {
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        }
    }
    else {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }


    object<Texture> tex;
    glGenTextures(1, &tex->_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex->_id);

    if (count > 1) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, tex->_antialias ? GL_LINEAR_MIPMAP_NEAREST : GL_NEAREST_MIPMAP_NEAREST);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, tex->_antialias ? GL_LINEAR : GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    tex->_width = width;
    tex->_height = height;
    tex->_pixelInfo = info;
    for (unsigned int i = 0; i < count; ++i, ++chunk) {
        unsigned char *data = chunk->data();
        GLsizei datalen = chunk->size();

        if (info->compressed()) {
            glCompressedTexImage2D(GL_TEXTURE_2D, i, info->glInternalFormat(), (GLsizei)width, (GLsizei)height, 0, datalen, data);
        }
        else {
            glTexImage2D(GL_TEXTURE_2D, i, info->glInternalFormat(), (GLsizei)width, (GLsizei)height, 0, info->glFormat(), info->glType(), data);
        }
        if (glGetError() != GL_NO_ERROR) {
            gv_error("gl load texture failed.");
            return nullptr;
        }
        width = std::max(width >> 1, 1u);
        height = std::max(height >> 1, 1u);
    }

    // shader
    //setGLProgram(GLProgramCache::getInstance()->getGLProgram(GLProgram::SHADER_NAME_POSITION_TEXTURE));
    return tex;

}

ptr<Texture> Texture::create(Image *image, PixelFormat format) noexcept {
    if (image->mipmaps().size() < 1) {
        return nullptr;
    }

    if (!image->pixelInfo()->support()) {
        gv_error("unsupport pixel format '%s'.", image->pixelInfo()->desc());
        return nullptr;
    }

    int maxTextureSize = Env::instance()->maxTextureSize();
    if (image->width() > (unsigned)maxTextureSize || image->height() > (unsigned)maxTextureSize) {
        gv_error("too big texture size width=%d height=%d.", image->width(), image->height());
        return nullptr;
    }

    ptr<PixelInfo> texPixelInfo;
    if (!image->pixelInfo()->compressed() && image->mipmaps().size() == 1) {
        if (format == PixelFormat::UNKNOWN) {
            format = Env::instance()->defaultPixelFormat;
        }
        texPixelInfo = PixelInfo::get(format);
        if (!texPixelInfo->support()) {
            gv_warning("unsupport texture pixel format '%s', texture use image pixel format '%s'.", texPixelInfo->desc(), image->pixelInfo()->desc());
            texPixelInfo = image->pixelInfo();
        }
        else if (texPixelInfo != image->pixelInfo()) {
            ptr<Chunk> chunk = image->pixelInfo()->convert(image->mipmaps()[0], format);
            if (!chunk) {
                gv_warning("can't convert pixel format from '%s' to '%s', texture use image pixel format.", image->pixelInfo()->desc(), texPixelInfo->desc());
                texPixelInfo = image->pixelInfo();
            }
            else {
                image->mipmaps()[0] = std::move(*chunk);
            }
        }
    }
    else {
        texPixelInfo = image->pixelInfo();
    }
    return create(image->mipmaps().data(), image->width(), image->height(), texPixelInfo, image->mipmaps().size());
}

GV_NS_END
