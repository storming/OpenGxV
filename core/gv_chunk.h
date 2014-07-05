#ifndef __GV_CHUNK_H__
#define __GV_CHUNK_H__

#include "gv_object.h"

GV_NS_BEGIN

class Chunk : public Object {
public:
    Chunk() : _data(), _size() {}
    Chunk(size_t size) noexcept : _size(size) {
        _data = (unsigned char*)std::malloc(size);
    }
    Chunk(const void *p, size_t size) noexcept : _size(size) {
        _data = (unsigned char*)std::malloc(size);
        std::memcpy(_data, p, _size);
    }
    Chunk(const Chunk &) = delete;
    Chunk(Chunk &&x) noexcept : _data(), _size() {
        swap(x);
    }
    ~Chunk() noexcept {
        if (_data) {
            std::free(_data);
        }
    }
    void swap(Chunk &x) noexcept {
        std::swap(_data, x._data);
        std::swap(_size, x._size);
    }
    unsigned char *data() const noexcept {
        return _data;
    }
    size_t size() const noexcept {
        return _size;
    }
    operator unsigned char*() noexcept {
        return _data;
    }
    Chunk &operator=(const  Chunk&) = delete;
    Chunk &operator=(Chunk &&x) noexcept {
        swap(x);
        return *this;
    }
    Chunk &operator=(std::nullptr_t) noexcept {
        if (_data) {
            std::free(_data);
            _size = 0;
        }
        return *this;
    }
    const unsigned char &operator[](size_t size) const noexcept {
        return _data[size];
    }
    unsigned char &operator[](size_t size) noexcept {
        return _data[size];
    }
private:
    unsigned char *_data;
    size_t _size;
};

GV_NS_END

#endif
