#ifndef __GV_FILE_H__
#define __GV_FILE_H__

#include "gv_platform.h"
#include "gv_unistr.h"
#include "gv_path.h"

GV_NS_BEGIN

enum class FileType {
    IMAGE_BEGIN,
    PNG = IMAGE_BEGIN,
    JPG,
    VPR,
    VPR_GZ,
    VPR_CCZ,
    BMP,
    TGA,
    TIFF,
    PKM,
    PPM,
    WEBP,
    IMAGE_END,
    PLIST = IMAGE_END,
    ANI,
    G9,
    LUA,
    TOC,
    UNKNOWN,
};

class File : public Object {
    GV_FRIEND_PTR();
public:
    static FileType type(const char *str, size_t size = 0) noexcept;
    static FileType type(const std::string &name) noexcept {
        return type(name.c_str(), name.size());
    }
    static FileType type(Path *path) noexcept {
        return type(path->ext());
    }
    static const std::string &root() noexcept {
        return _root;
    }
    static void root(const std::string &path) noexcept {
        _root = path;
    }
    static ptr<File> load(const ptr<Path> &path) noexcept;
public:
    ~File() noexcept;
    size_t size() const noexcept {
        return _size;
    }
    const unsigned char *data() const noexcept {
        return _data;
    }
    const std::string &path() const noexcept {
        return _path;
    }
    int read(void *buf, size_t size) noexcept;
private:
    File(const ptr<Path> &path) noexcept;
private:
    static std::string _root;
    std::string _path;
    unsigned char *_data;
    size_t _size;
    size_t _pos;
};

GV_NS_END


#endif
