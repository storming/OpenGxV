#include "opengv.h"

#include <cstdlib>
#include "gv_log.h"


GV_NS_BEGIN

Log::Log() : _chunk(8192), _pos() {}

void Log::begin(LogLevel level) noexcept {
    _level = level;
    _pos = 0;
}

void Log::end() noexcept {
    if (_pos > 0) {
        _chunk[_pos++] = '\n';
        _chunk[_pos++] = '\0';
        fputs((char*)_chunk.data(), stderr);
        fflush(stderr);
    }
}

void Log::vprint(const char *fmt, va_list ap) noexcept {
    if (_pos < _chunk.size() - 1) {
        _pos += std::vsnprintf((char*)_chunk.data() + _pos, _chunk.size() - _pos - 1, fmt, ap);
    }
}

void Log::print(const char *fmt, ...) noexcept {
    va_list ap;
    va_start(ap, fmt);
    vprint(fmt, ap);
    va_end(ap);
}

GV_NS_END

