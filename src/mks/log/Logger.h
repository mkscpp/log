//
// Created by Michal Němec on 01/01/2020.
//

#ifndef MKS_LOGGER_H
#define MKS_LOGGER_H

#include "../log.h"
#include <chrono>
#include <functional>
#include <unordered_map>

namespace mks {

enum class log_type : int {
    MKS_LOG_TYPE_NORMAL,
    MKS_LOG_TYPE_DEBUG,
    MKS_LOG_TYPE_UNKNOWN
};

struct LogItem {
    std::chrono::system_clock::time_point start;
    log_type type = log_type::MKS_LOG_TYPE_UNKNOWN;
    log::timestamp timestamp;
    int level = -1;
    int line = -1;
    std::string tid;
    int64_t pid;
    const char* file = nullptr;
    const char* function = nullptr;
    const char* message = nullptr;
};

using log_cb = std::function<void(LogItem&)>;

class Logger : public BaseLogger {
    log_cb cb_ = log_cb();

public:
    Logger();

    virtual ~Logger() {
        if(log::log_ptr() == this) {
            log::reset_logger();
            // MKS_LOG_W("destroyed with active global pointer, changed logger to default");
        }
    }

    void set_on_log_listener(log_cb cb);
    void log(int level, int line, const char* file, const char* function, const char* tag, const char* message) override;

    static void default_log(const LogItem& item);
    static void
    default_log(const LogItem& item, const std::unordered_map<std::string, std::string>& tid_2_str_map);
};

#define MKS_LOG_FORMAT_DEFAULT "%x%l%o%d %t %p %h [%f:%g] %s: %m%y\n"

class LogFormat {
    enum Type {
        MKS_LOG_FORMAT_LOG_COLOR_START,
        MKS_LOG_FORMAT_LOG_COLOR_END,
        MKS_LOG_FORMAT_LOG_LEVEL,
        MKS_LOG_FORMAT_LOG_DAY,
        MKS_LOG_FORMAT_LOG_MONTH,
        MKS_LOG_FORMAT_LOG_TIMESTAMP,
        MKS_LOG_FORMAT_LOG_PID,
        MKS_LOG_FORMAT_LOG_TID,
        MKS_LOG_FORMAT_LOG_FILE,
        MKS_LOG_FORMAT_LOG_LINE,
        MKS_LOG_FORMAT_LOG_FUNCTION,
        MKS_LOG_FORMAT_LOG_MESSAGE,
    };

    const std::unordered_map<Type, char> format_map = {
        {MKS_LOG_FORMAT_LOG_COLOR_START, 'x'},
        {MKS_LOG_FORMAT_LOG_COLOR_END, 'y'},
        {MKS_LOG_FORMAT_LOG_LEVEL, 'l'},
        {MKS_LOG_FORMAT_LOG_DAY, 'd'},
        {MKS_LOG_FORMAT_LOG_MONTH, 'o'},
        {MKS_LOG_FORMAT_LOG_TIMESTAMP, 't'},
        {MKS_LOG_FORMAT_LOG_PID, 'p'},
        {MKS_LOG_FORMAT_LOG_TID, 'h'},
        {MKS_LOG_FORMAT_LOG_FILE, 'f'},
        {MKS_LOG_FORMAT_LOG_LINE, 'g'},
        {MKS_LOG_FORMAT_LOG_FUNCTION, 's'},
        {MKS_LOG_FORMAT_LOG_MESSAGE, 'm'}};

    static int
    replace(std::string& subject, const std::string& search, const std::string& replace);

public:
    // used to enable/disable colors based on capabilities of the current terminal
    bool enable_colors = true;

    std::string format(
        const char* format,
        mks::LogItem& item,
        const std::unordered_map<std::string, std::string>& tid_2_str_map = {});
};

} // namespace mks

#endif // MKS_LOGGER_H
