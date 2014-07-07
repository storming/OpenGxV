#include "opengxv.h"
#include "windows.h"
#include "gv_file.h"

GV_NS_BEGIN
#include "gv_filetype.inl"
FileType File::type(const char *str, size_t size) noexcept {
    auto *desc = FileTypeHash::get(str, size);
    return desc ? desc->type : FileType::UNKNOWN;
}

std::string File::_root("data/");

File::File(const ptr<Path> &path) noexcept : _path(path->tostring()), _data(), _size() {
}

File::~File() noexcept {
    if (_data) {
        std::free(_data);
    }
}

ptr<File> File::load(const ptr<Path> &path) noexcept {
    object<File> file(path);

    unsigned char *buffer = nullptr;

    size_t size = 0;

    WCHAR wszBuf[4096] = { 0 };
    std::string fullPath = _root + file->_path;
    MultiByteToWideChar(CP_UTF8, 0, fullPath.c_str(), -1, wszBuf, sizeof(wszBuf) / sizeof(wszBuf[0]));

    HANDLE handle = ::CreateFileW(wszBuf, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (handle == INVALID_HANDLE_VALUE) {
        return nullptr;
    }

    size = ::GetFileSize(handle, NULL);

    file->_data = (unsigned char*)malloc(size + 1);
    file->_data[size] = '\0';

    DWORD sizeRead = 0;
    BOOL successed = ::ReadFile(handle, file->_data, size, &sizeRead, NULL);
    ::CloseHandle(handle);

    if (!successed) {
        return nullptr;
    }
    file->_size = sizeRead;
    file->_pos = 0;
    return file;
}

int File::read(void *buf, size_t size) noexcept {
    if (_pos + size > _size) {
        size = _size - _pos;
    }
    if (size) {
        memcpy(buf, _data + _pos, size);
        _pos += size;
    }
    return size;
}

GV_NS_END
