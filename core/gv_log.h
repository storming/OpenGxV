#ifndef __GV_LOG_H__
#define __GV_LOG_H__

#include <cstdarg>

#include "gv_chunk.h"
#include "gv_singleton.h"

GV_NS_BEGIN

enum class LogLevel {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
};

class Log : public Object, public singleton<Log> {
    friend class Object;
public:
    void begin(LogLevel level) noexcept;
    void end() noexcept;
    void vprint(const char *fmt, va_list ap) noexcept;
    void print(const char *fmt, ...) noexcept;

    void operator()(LogLevel level, const char *fmt, ...) noexcept {
        va_list ap;
        va_start(ap, fmt);
        begin(level);
        vprint(fmt, ap);
        end();
        va_end(ap);
    }
private:
    Log();

    Chunk _chunk;
    size_t _pos;
    LogLevel _level;
};

GV_NS_END

#define gv_debug(fmt, ...)   (*gv::Log::instance())(gv::LogLevel::LOG_DEBUG,   fmt, ##__VA_ARGS__)
#define gv_info(fmt, ...)    (*gv::Log::instance())(gv::LogLevel::LOG_INFO,    fmt, ##__VA_ARGS__)
#define gv_warning(fmt, ...) (*gv::Log::instance())(gv::LogLevel::LOG_WARNING, fmt, ##__VA_ARGS__)
#define gv_error(fmt, ...)   (*gv::Log::instance())(gv::LogLevel::LOG_ERROR,   fmt, ##__VA_ARGS__)
#define gv_fail(fmt, ...) do {                                            \
    gv_error(fmt, ##__VA_ARGS__);                                         \
    std::abort();                                                         \
} while (0)

#ifdef GV_DEBUG
#define gv_assert(x, fmt, ...) do {                                       \
    if (!(x)) {                                                           \
        gv::Log::instance()->begin(gv::LogLevel::LOG_ERROR);              \
        gv::Log::instance()->print("assert '%s' failed, at %s@(%lu:%s).", \
            #x, __FUNCTION__, __LINE__, __FILE__);                        \
        gv::Log::instance()->print(fmt, ##__VA_ARGS__);                   \
        gv::Log::instance()->end();                                       \
        std::abort();                                                     \
    }                                                                     \
} while (0)
#endif

#define gv_valid(x, fmt, ...) do {                                        \
    if (!(x)) {                                                           \
        gv::Log::instance()->begin(gv::LogLevel::LOG_ERROR);              \
        gv::Log::instance()->print("valid '%s' failed, at %s@(%lu:%s).",  \
            #x, __FUNCTION__, __LINE__, __FILE__);                        \
        gv::Log::instance()->print(fmt, ##__VA_ARGS__);                   \
        gv::Log::instance()->end();                                       \
        std::abort();                                                     \
    }                                                                     \
} while (0)

#endif
