//
// Created by Michal Němec on 01/01/2020.
//

#ifndef MKS_LOG_H
#define MKS_LOG_H

#include <cassert>

#include <fmt/format.h>

#include <iostream>

#ifdef __linux__
#include <sys/syscall.h>
#include <unistd.h>
#elif __APPLE__
#include <sys/syscall.h>
#include <unistd.h>
#elif _WIN32
// in cpp.file
#else
#include <thread>
#endif

#include <cstring>
#include "log/ColorModifier.h"

// / for unix, \\ for windows
#define MKS_FILENAME(x)                                                                            \
    (strrchr(x, '/') ? strrchr(x, '/') + 1 : (strrchr(x, '\\') ? strrchr(x, '\\') + 1 : x))
#define MKS_METHOD(x) mks::method_name(x)

#define MKS_ENABLE_LOG
#define MKS_USE_DEBUG_LOG
#define MKS_LOG_PTR(x) static_cast<void *>(x)

#if defined(__GNUC__) || defined(__clang__)
#define MKS_FUNCTION __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
#define MKS_FUNCTION __FUNCSIG__
#elif defined(__SUNPRO_CC)
#define MKS_FUNCTION __func__
#else
#define MKS_FUNCTION __FUNCTION__
#endif

#ifndef NDEBUG
#define MKS_ASSERT(expr)                                                                           \
    do {                                                                                           \
        if(!static_cast<bool>(expr)) {                                                             \
            mks::log::a(mks::log::log_ptr(), #expr, __LINE__, __FILE__, MKS_FUNCTION);                                  \
        }                                                                                          \
        assert(expr);                                                                              \
    } while(false)
#define MKS_LASSERT(logger, expr)                                                                           \
    do {                                                                                           \
        if(!static_cast<bool>(expr)) {                                                             \
            mks::log::a(logger, #expr, __LINE__, __FILE__, MKS_FUNCTION);                                  \
        }                                                                                          \
        assert(expr);                                                                              \
    } while(false)
#else
#define MKS_ASSERT(tag, expr) assert(expr)
#define MKS_LASSERT(logger, expr) assert(expr)
#endif

#ifdef MKS_ENABLE_LOG

namespace mks {

std::string tid();
int64_t pid();

bool enable_terminal_colors();
std::string method_name(const char *fn);

struct BaseLogger {
    virtual void log(int level, int line, const char *file, const char *function, const char *tag, const char *message) = 0;
};

struct StdLogger : public BaseLogger {
    void log(int level, int line, const char *file, const char *function, const char *tag, const char *message) override;
};

namespace log {

struct timestamp {
    uint8_t month;
    uint8_t day;
    std::string date;
    std::string time;
    std::string timezone;
    std::string timezone_offset;
};

timestamp current_detail_timestamp();

BaseLogger *log_ptr();
void set_logger(BaseLogger *logger);
void reset_logger();

enum class level : int {
    MKS_LOG_LEVEL_DEBUG = 0,
    MKS_LOG_LEVEL_ERROR,
    MKS_LOG_LEVEL_FATAL,
    MKS_LOG_LEVEL_INFO,
    MKS_LOG_LEVEL_VERBOSE,
    MKS_LOG_LEVEL_WARN,
    MKS_LOG_LEVEL_ASSERT,
    MKS_LOG_LEVEL_MAX_SIZE
};

log::Modifier level_modifier(int level);
std::string level_str(int level);

void a(mks::BaseLogger* logger, const char *expr, int line, const char *file, const char *function);

template <typename... Args>
void log_debug(mks::BaseLogger *logger, const char *tag, int line, const char *file, const char *function, const std::string &format, Args &&... args) {
    MKS_ASSERT(logger != nullptr);
    auto str = fmt::format(format, std::forward<Args>(args)...);
    logger->log(static_cast<int>(level::MKS_LOG_LEVEL_DEBUG), line, file, function, tag, str.c_str());
}

template <typename... Args>
void log_error(mks::BaseLogger *logger, const char *tag, int line, const char *file, const char *function, const std::string &format, Args &&... args) {
    MKS_ASSERT(logger != nullptr);
    auto str = fmt::format(format, std::forward<Args>(args)...);
    logger->log(static_cast<int>(level::MKS_LOG_LEVEL_ERROR), line, file, function, tag, str.c_str());
}

template <typename... Args>
void log_fatal(mks::BaseLogger *logger, const char *tag, int line, const char *file, const char *function, const std::string &format, Args &&... args) {
    MKS_ASSERT(logger != nullptr);
    auto str = fmt::format(format, std::forward<Args>(args)...);
    logger->log(static_cast<int>(level::MKS_LOG_LEVEL_FATAL), line, file, function, tag, str.c_str());
}

template <typename... Args>
void log_info(mks::BaseLogger *logger, const char *tag, int line, const char *file, const char *function, const std::string &format, Args &&... args) {
    MKS_ASSERT(logger != nullptr);
    auto str = fmt::format(format, std::forward<Args>(args)...);
    logger->log(static_cast<int>(level::MKS_LOG_LEVEL_INFO), line, file, function, tag, str.c_str());
}

template <typename... Args>
void log_verbose(mks::BaseLogger *logger, const char *tag, int line, const char *file, const char *function, const std::string &format, Args &&... args) {
    MKS_ASSERT(logger != nullptr);
    auto str = fmt::format(format, std::forward<Args>(args)...);
    logger->log(static_cast<int>(level::MKS_LOG_LEVEL_VERBOSE), line, file, function, tag, str.c_str());
}

template <typename... Args>
void log_warning(mks::BaseLogger *logger, const char *tag, int line, const char *file, const char *function, const std::string &format, Args &&... args) {
    MKS_ASSERT(logger != nullptr);
    auto str = fmt::format(format, std::forward<Args>(args)...);
    logger->log(static_cast<int>(level::MKS_LOG_LEVEL_WARN), line, file, function, tag, str.c_str());
}

} // namespace log

} // namespace mks

/*
 * EXTERNAL LOGGER MIRRORED MACROS
 */

/*
 * Logger (L) Normal logging
 */
#define MKS_LOG_LD(logger, ...) mks::log::log_debug(static_cast<mks::BaseLogger*>(logger), "", __LINE__, __FILE__, MKS_FUNCTION, __VA_ARGS__)
#define MKS_LOG_LE(logger, ...) mks::log::log_error(static_cast<mks::BaseLogger*>(logger), "", __LINE__, __FILE__, MKS_FUNCTION, __VA_ARGS__)
#define MKS_LOG_LF(logger, ...) mks::log::log_fatal(static_cast<mks::BaseLogger*>(logger), "", __LINE__, __FILE__, MKS_FUNCTION, __VA_ARGS__)
#define MKS_LOG_LI(logger, ...) mks::log::log_info(static_cast<mks::BaseLogger*>(logger), "", __LINE__, __FILE__, MKS_FUNCTION, __VA_ARGS__)
#define MKS_LOG_LV(logger, ...) mks::log::log_verbose(static_cast<mks::BaseLogger*>(logger), "", __LINE__, __FILE__, MKS_FUNCTION, __VA_ARGS__)
#define MKS_LOG_LW(logger, ...) mks::log::log_warning(static_cast<mks::BaseLogger*>(logger), "", __LINE__, __FILE__, MKS_FUNCTION, __VA_ARGS__)

/*
 * Logger-Condition (LC) logging
 */
#define MKS_LOG_LCD(logger, condition, ...) do { if(static_cast<bool>(condition)) mks::log::log_debug(static_cast<mks::BaseLogger*>(logger), "", __LINE__, __FILE__, MKS_FUNCTION, __VA_ARGS__); } while(0)
#define MKS_LOG_LCE(logger, condition, ...) do { if(static_cast<bool>(condition)) mks::log::log_error(static_cast<mks::BaseLogger*>(logger), "", __LINE__, __FILE__, MKS_FUNCTION, __VA_ARGS__); } while(0)
#define MKS_LOG_LCF(logger, condition, ...) do { if(static_cast<bool>(condition)) mks::log::log_fatal(static_cast<mks::BaseLogger*>(logger), "", __LINE__, __FILE__, MKS_FUNCTION, __VA_ARGS__); } while(0)
#define MKS_LOG_LCI(logger, condition, ...) do { if(static_cast<bool>(condition)) mks::log::log_info(static_cast<mks::BaseLogger*>(logger), "", __LINE__, __FILE__, MKS_FUNCTION, __VA_ARGS__); } while(0)
#define MKS_LOG_LCV(logger, condition, ...) do { if(static_cast<bool>(condition)) mks::log::log_verbose(static_cast<mks::BaseLogger*>(logger), "", __LINE__, __FILE__, MKS_FUNCTION, __VA_ARGS__); } while(0)
#define MKS_LOG_LCW(logger, condition, ...) do { if(static_cast<bool>(condition)) mks::log::log_warning(static_cast<mks::BaseLogger*>(logger), "", __LINE__, __FILE__, MKS_FUNCTION, __VA_ARGS__); } while(0)

/*
 * Logger-Stream (LS) logging
 */
#define MKS_LOG_LSD(logger, x) do { \
        std::ostringstream oss; \
        (oss << x); \
        mks::log::log_debug(static_cast<mks::BaseLogger*>(logger), "", __LINE__, __FILE__, MKS_FUNCTION, oss.str()); \
    } while(0)

#define MKS_LOG_LSE(logger, x) do { \
        std::ostringstream oss; \
        (oss << x); \
        mks::log::log_error(static_cast<mks::BaseLogger*>(logger), "", __LINE__, __FILE__, MKS_FUNCTION, oss.str()); \
    } while(0)

#define MKS_LOG_LSF(logger, x) do { \
        std::ostringstream oss; \
        (oss << x); \
        mks::log::log_fatal(static_cast<mks::BaseLogger*>(logger), "", __LINE__, __FILE__, MKS_FUNCTION, oss.str()); \
    } while(0)

#define MKS_LOG_LSI(logger, x) do { \
        std::ostringstream oss; \
        (oss << x); \
        mks::log::log_info(static_cast<mks::BaseLogger*>(logger), "", __LINE__, __FILE__, MKS_FUNCTION, oss.str()); \
    } while(0)

#define MKS_LOG_LSV(logger, x) do { \
        std::ostringstream oss; \
        (oss << x); \
        mks::log::log_verbose(static_cast<mks::BaseLogger*>(logger), "", __LINE__, __FILE__, MKS_FUNCTION, oss.str()); \
    } while(0)

#define MKS_LOG_LSW(logger, x) do { \
        std::ostringstream oss; \
        (oss << x); \
        mks::log::log_warning(static_cast<mks::BaseLogger*>(logger), "", __LINE__, __FILE__, MKS_FUNCTION, oss.str()); \
    } while(0)

/*
 * Logger-Condition-Stream (LCS) logging
 */
#define MKS_LOG_LCSD(logger, condition, x) do { \
        if(!static_cast<bool>(condition)) break; \
        std::ostringstream oss; \
        (oss << x); \
        mks::log::log_debug(static_cast<mks::BaseLogger*>(logger), "", __LINE__, __FILE__, MKS_FUNCTION, oss.str()); \
    } while(0)

#define MKS_LOG_LCSE(logger, condition, x) do { \
        if(!static_cast<bool>(condition)) break; \
        std::ostringstream oss; \
        (oss << x); \
        mks::log::log_error(static_cast<mks::BaseLogger*>(logger), "", __LINE__, __FILE__, MKS_FUNCTION, oss.str()); \
    } while(0)

#define MKS_LOG_LCSF(logger, condition, x) do { \
        if(!static_cast<bool>(condition)) break; \
        std::ostringstream oss; \
        (oss << x); \
        mks::log::log_fatal(static_cast<mks::BaseLogger*>(logger), "", __LINE__, __FILE__, MKS_FUNCTION, oss.str()); \
    } while(0)

#define MKS_LOG_LCSI(logger, condition, x) do { \
        if(!static_cast<bool>(condition)) break; \
        std::ostringstream oss; \
        (oss << x); \
        mks::log::log_info(static_cast<mks::BaseLogger*>(logger), "", __LINE__, __FILE__, MKS_FUNCTION, oss.str()); \
    } while(0)

#define MKS_LOG_LCSV(logger, condition, x) do { \
        if(!static_cast<bool>(condition)) break; \
        std::ostringstream oss; \
        (oss << x); \
        mks::log::log_verbose(static_cast<mks::BaseLogger*>(logger), "", __LINE__, __FILE__, MKS_FUNCTION, oss.str()); \
    } while(0)

#define MKS_LOG_LCSW(logger, condition, x) do { \
        if(!static_cast<bool>(condition)) break; \
        std::ostringstream oss; \
        (oss << x); \
        mks::log::log_warning(static_cast<mks::BaseLogger*>(logger), "", __LINE__, __FILE__, MKS_FUNCTION, oss.str()); \
    } while(0)

/*
 * Default logger macros
 */

/*
 * Normal logging
 */
#define MKS_LOG_D(...) MKS_LOG_LD(mks::log::log_ptr(), __VA_ARGS__)
#define MKS_LOG_E(...) MKS_LOG_LE(mks::log::log_ptr(), __VA_ARGS__)
#define MKS_LOG_F(...) MKS_LOG_LF(mks::log::log_ptr(), __VA_ARGS__)
#define MKS_LOG_I(...) MKS_LOG_LI(mks::log::log_ptr(), __VA_ARGS__)
#define MKS_LOG_V(...) MKS_LOG_LV(mks::log::log_ptr(), __VA_ARGS__)
#define MKS_LOG_W(...) MKS_LOG_LW(mks::log::log_ptr(), __VA_ARGS__)

/*
 * Condition (C) logging
 */
#define MKS_LOG_CD(condition, ...) MKS_LOG_LCD(mks::log::log_ptr(), condition, __VA_ARGS__)
#define MKS_LOG_CE(condition, ...) MKS_LOG_LCE(mks::log::log_ptr(), condition, __VA_ARGS__)
#define MKS_LOG_CF(condition, ...) MKS_LOG_LCF(mks::log::log_ptr(), condition, __VA_ARGS__)
#define MKS_LOG_CI(condition, ...) MKS_LOG_LCI(mks::log::log_ptr(), condition, __VA_ARGS__)
#define MKS_LOG_CV(condition, ...) MKS_LOG_LCV(mks::log::log_ptr(), condition, __VA_ARGS__)
#define MKS_LOG_CW(condition, ...) MKS_LOG_LCW(mks::log::log_ptr(), condition, __VA_ARGS__)
/*
 * Stream (S) logging
 */
#define MKS_LOG_SD(x) MKS_LOG_LSD(mks::log::log_ptr(), x)
#define MKS_LOG_SE(x) MKS_LOG_LSE(mks::log::log_ptr(), x)
#define MKS_LOG_SF(x) MKS_LOG_LSF(mks::log::log_ptr(), x)
#define MKS_LOG_SI(x) MKS_LOG_LSI(mks::log::log_ptr(), x)
#define MKS_LOG_SV(x) MKS_LOG_LSV(mks::log::log_ptr(), x)
#define MKS_LOG_SW(x) MKS_LOG_LSW(mks::log::log_ptr(), x)
/*
 * Condition-Stream (CS) logging
 */
#define MKS_LOG_CSD(condition, x) MKS_LOG_LCSD(mks::log::log_ptr(), condition, x)
#define MKS_LOG_CSE(condition, x) MKS_LOG_LCSE(mks::log::log_ptr(), condition, x)
#define MKS_LOG_CSF(condition, x) MKS_LOG_LCSF(mks::log::log_ptr(), condition, x)
#define MKS_LOG_CSI(condition, x) MKS_LOG_LCSI(mks::log::log_ptr(), condition, x)
#define MKS_LOG_CSV(condition, x) MKS_LOG_LCSV(mks::log::log_ptr(), condition, x)
#define MKS_LOG_CSW(condition, x) MKS_LOG_LCSW(mks::log::log_ptr(), condition, x)


#else
#define MKS_LOG_D(...)
#define MKS_LOG_E(...)
#define MKS_LOG_F(...)
#define MKS_LOG_I(...)
#define MKS_LOG_V(...)
#define MKS_LOG_W(...)

#define MKS_LOG_CD(...)
#define MKS_LOG_CE(...)
#define MKS_LOG_CF(...)
#define MKS_LOG_CI(...)
#define MKS_LOG_CV(...)
#define MKS_LOG_CW(...)

#define MKS_LOG_SD(...)
#define MKS_LOG_SE(...)
#define MKS_LOG_SF(...)
#define MKS_LOG_SI(...)
#define MKS_LOG_SV(...)
#define MKS_LOG_SW(...)

#define MKS_LOG_CSD(...)
#define MKS_LOG_CSE(...)
#define MKS_LOG_CSF(...)
#define MKS_LOG_CSI(...)
#define MKS_LOG_CSV(...)
#define MKS_LOG_CSW(...)

#define MKS_LOG_LD(...)
#define MKS_LOG_LE(...)
#define MKS_LOG_LF(...)
#define MKS_LOG_LI(...)
#define MKS_LOG_LV(...)
#define MKS_LOG_LW(...)

#define MKS_LOG_LCD(...)
#define MKS_LOG_LCE(...)
#define MKS_LOG_LCF(...)
#define MKS_LOG_LCI(...)
#define MKS_LOG_LCV(...)
#define MKS_LOG_LCW(...)

#define MKS_LOG_LSD(...)
#define MKS_LOG_LSE(...)
#define MKS_LOG_LSF(...)
#define MKS_LOG_LSI(...)
#define MKS_LOG_LSV(...)
#define MKS_LOG_LSW(...)

#define MKS_LOG_LCSD(...)
#define MKS_LOG_LCSE(...)
#define MKS_LOG_LCSF(...)
#define MKS_LOG_LCSI(...)
#define MKS_LOG_LCSV(...)
#define MKS_LOG_LCSW(...)
#endif

#endif // MKS_LOG_H
