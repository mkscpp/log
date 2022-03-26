//
// Created by Michal Němec on 01/01/2020.
//

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "log.h"
#include "log/ColorModifier.h"

#ifdef __APPLE__
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_12
#include <pthread.h>
#endif
#endif

#ifdef _WIN32
#include <processthreadsapi.h>
#include <windows.h>
#endif

using namespace mks;

namespace {
mks::StdLogger _std_log{};
mks::BaseLogger* _log = static_cast<mks::BaseLogger*>(&_std_log);
} // namespace


log::timestamp mks::log::current_detail_timestamp() {
#if defined(__APPLE__) || defined(_WIN32)
    auto now = std::chrono::system_clock::now();
#else
    auto now = std::chrono::high_resolution_clock::now();
#endif
    auto time_now = std::chrono::system_clock::to_time_t(now);
    auto tm = *std::localtime(&time_now);

    auto epoch = now.time_since_epoch();
    auto us = std::chrono::duration_cast<std::chrono::microseconds>(epoch).count() % 1000000LL;

    timestamp tms{};

    tms.month = 1 + tm.tm_mon;
    tms.day = tm.tm_mday;
    {
        std::ostringstream oss;
        oss << std::put_time(&tm, "%F");
        tms.date = oss.str();
    }
    {
        std::ostringstream oss;
#ifdef _WIN32
        oss << std::put_time(&tm, "%X.");
#else
        oss << std::put_time(&tm, "%T.");
#endif

        oss << fmt::format("{:0>6}", us);

        tms.time = oss.str();
    }
    {
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Z");
        tms.timezone = oss.str();
    }
    {
        std::ostringstream oss;
        oss << std::put_time(&tm, "%z");
        tms.timezone_offset = oss.str();
    }
    return tms;
}

mks::BaseLogger* mks::log::log_ptr() {
    return _log;
}

void mks::log::set_logger(BaseLogger* logger) {
    _log = logger;
}

void mks::log::reset_logger() {
    _log = static_cast<mks::BaseLogger*>(&_std_log);
}

void StdLogger::log(int level, int line, const char* file, const char* function, const char* tag, const char* message) {
    // output stream
    auto& out = std::cerr;

    // rest of logging formatting
    static log::Modifier def(log::FG_DEFAULT);
    auto tm = mks::log::current_detail_timestamp();
    out << log::level_modifier(level) << mks::log::level_str(level)
        << fmt::format("{:0>2}", tm.month) << fmt::format("{:0>2}", tm.day)
        << " " << tm.time << " " << mks::pid() << " " << mks::tid() << " ["
        << MKS_FILENAME(file) << ":" << line << "] " << MKS_METHOD(function)
        << ": " << message << def << "\n";
}

std::string mks::log::level_str(int l) {
    if(l >= 0 && l < static_cast<int>(level::MKS_LOG_LEVEL_MAX_SIZE)) {
        switch(static_cast<level>(l)) {
            case level::MKS_LOG_LEVEL_DEBUG:
                return "D";
            case level::MKS_LOG_LEVEL_ERROR:
                return "E";
            case level::MKS_LOG_LEVEL_INFO:
                return "I";
            case level::MKS_LOG_LEVEL_VERBOSE:
                return "V";
            case level::MKS_LOG_LEVEL_WARN:
                return "W";
            case level::MKS_LOG_LEVEL_FATAL:
                return "F";
            case level::MKS_LOG_LEVEL_ASSERT:
                return "A";
            case level::MKS_LOG_LEVEL_MAX_SIZE:
                MKS_ASSERT(0); // should not happen
                break;
            default:
                // throw it into default number handling
                break;
        }
    }
    return fmt::format("{}", l);
}

mks::log::Modifier mks::log::level_modifier(int l) {
    if(l >= 0 && l < static_cast<int>(level::MKS_LOG_LEVEL_MAX_SIZE)) {
        switch(static_cast<level>(l)) {
            case level::MKS_LOG_LEVEL_DEBUG:
                return {log::FG_DEFAULT};
            case level::MKS_LOG_LEVEL_ERROR:
                return {log::FG_RED};
            case level::MKS_LOG_LEVEL_INFO:
                return {log::FG_GREEN};
            case level::MKS_LOG_LEVEL_VERBOSE:
                return {log::FG_BLUE};
            case level::MKS_LOG_LEVEL_WARN:
                return {log::FG_MAGENTA};
            case level::MKS_LOG_LEVEL_ASSERT:
                return {log::FG_RED};
            case level::MKS_LOG_LEVEL_FATAL:
                return {log::FG_RED};
            case level::MKS_LOG_LEVEL_MAX_SIZE:
                MKS_ASSERT(0); // should not happen
                break;
            default:
                // throw it into default number handling
                break;
        }
    }
    return {log::FG_DEFAULT};
}

void mks::log::a(mks::BaseLogger* logger, const char* expr, int line, const char* file, const char* function) {
    if(logger == nullptr) {
        // default to base logger
        logger = log_ptr();
        MKS_ASSERT(logger != nullptr);
    }
    auto str = fmt::format("{}: Assertion \'{}\' failed", function, expr);
    logger->log(static_cast<int>(level::MKS_LOG_LEVEL_ASSERT), line, file, function, "assert", str.c_str());
}

std::string mks::tid() {
#ifdef __NR_gettid
    uint64_t th_id = syscall(__NR_gettid);
    return fmt::format("{}", th_id);
#elif SYS_thread_selfid
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_12
    uint64_t tid64;
    pthread_threadid_np(nullptr, &tid64);
    return fmt::format("{}", tid64);
#else
    uint64_t th_id = syscall(SYS_thread_selfid);
    return fmt::format("{}", th_id);
#endif
#elif defined(_WIN32)
    auto th_id = GetCurrentThreadId();
    return fmt::format("{}", th_id);
#else
    auto this_th_id = std::this_thread::get_id();
    std::ostringstream oss;
    oss << this_th_id;
    return oss.str();
#endif
}

int64_t mks::pid() {
#ifdef _WIN32
    return GetCurrentProcessId();
#else
    return getpid();
#endif
}

bool mks::enable_terminal_colors() {
#ifdef _WIN32
    // enable color output windows
    // in case of failure disable colors since printing escape characters will make everything unreadable
    // its better tu just omit them and still have text without color
    HANDLE win_output_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD win_terminal_mode;
    if(GetConsoleMode(win_output_handle, &win_terminal_mode)) {
        win_terminal_mode |= ENABLE_PROCESSED_INPUT;
        // process as VT100 terminal
        win_terminal_mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        if(SetConsoleMode(win_output_handle, win_terminal_mode)) {
            return false;
        }
    }
#else
    // assume that on using the terminal has VT100 colors by default
#endif
    return true;
}

std::string mks::method_name(const char* fn) {
    // const char* end_ptr = nullptr;
    const char* start_ptr = nullptr;
    const char* ptr = fn;
    while(*ptr != '\0') {
        if(*ptr == '(') {
            // end_ptr = ptr;
            break;
        }
        ptr++;
    }
    if(*ptr == '\0') {
        return fn;
    }
    int len = 0;
    while(ptr != fn) {
        if(*ptr == ':' || *ptr == ' ' || *ptr == '<') {
            start_ptr = ptr + 1;
            len--;
            break;
        }
        len++;
        ptr--;
    }
    if(ptr == fn) {
        start_ptr = fn;
    }
    auto ret = std::string(start_ptr, len);
    if(ret.empty())
        return fn;
    return ret;
}