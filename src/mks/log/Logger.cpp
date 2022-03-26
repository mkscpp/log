//
// Created by Michal Němec on 01/01/2020.
//

#include "Logger.h"
#include <chrono>
#include <sstream>

#ifdef _WIN32
#include <processthreadsapi.h>
#include <windows.h>
#endif

using namespace mks;


Logger::Logger() = default;

void Logger::set_on_log_listener(log_cb cb) {
    cb_ = std::move(cb);
}

void Logger::log(int level, int line, const char* file, const char* function, const char* tag, const char* message) {
    LogItem item{};
    item.start = std::chrono::system_clock::now();
    item.tid = mks::tid();
    item.pid = mks::pid();
    item.timestamp = mks::log::current_detail_timestamp();
    item.type = log_type::MKS_LOG_TYPE_DEBUG;
    item.message = message;
    item.level = level;
    item.line = line;
    item.function = function;
    item.file = file;
    cb_(item);
}

void Logger::default_log(const LogItem& item, const std::unordered_map<std::string, std::string>& tid_2_str_map) {

    std::string tid_str;
    auto it = tid_2_str_map.find(item.tid);
    if(it != tid_2_str_map.end()) {
        tid_str = it->second;
    } else {
        tid_str = item.tid;
    }

    static log::Modifier def(log::FG_DEFAULT);
    std::cerr << log::level_modifier(item.level) << mks::log::level_str(item.level)
              << fmt::format("{:0>2}", item.timestamp.month)
              << fmt::format("{:0>2}", item.timestamp.day) << " "
              << item.timestamp.time << " ";
#ifdef MKS_USE_DEBUG_LOG
    std::cerr << item.pid << " " << tid_str << " [" << MKS_FILENAME(item.file) << ":"
              << item.line << "] " << MKS_METHOD(item.function) << ": " << item.message;
#else
    std::cerr << "(" << item.level << ")[" << item.tag << "] " << item.message;
#endif
    std::cerr << def << "\n";
}

void Logger::default_log(const LogItem& item) {
    static log::Modifier def(log::FG_DEFAULT);
    std::cout << log::level_modifier(item.level) << mks::log::level_str(item.level)
              << fmt::format("{:0>2}", item.timestamp.month)
              << fmt::format("{:0>2}", item.timestamp.day) << " "
              << item.timestamp.time << " ";
#ifdef MKS_USE_DEBUG_LOG
    std::cout << item.pid << " " << item.tid << " [" << MKS_FILENAME(item.file)
              << ":" << item.line << "] " << MKS_METHOD(item.function) << ": "
              << item.message;
#else
    std::cerr << "(" << item.level << ")[" << item.tag << "] " << item.message;
#endif
    std::cout << def << "\n";
}

int LogFormat::replace(std::string& subject, const std::string& search, const std::string& replace) {
    int count = 0;
    size_t pos = 0;
    while((pos = subject.find(search, pos)) != std::string::npos) {
        subject.replace(pos, search.length(), replace);
        pos += replace.length();
        count++;
    }
    return count;
}

std::string LogFormat::format(
    const char* format,
    mks::LogItem& item,
    const std::unordered_map<std::string, std::string>& tid_2_str_map) {
    std::string out = format;

    char rep[3] = {'%', ' ', '\0'};
    // replace functions requires string
    std::string rep_str = rep;

    for(auto& it : format_map) {
        auto type = it.first;
        rep_str[1] = it.second; // rep = %<mod>\0
        switch(type) {
            case MKS_LOG_FORMAT_LOG_LEVEL:
                replace(out, rep_str, mks::log::level_str(item.level));
                break;
            case MKS_LOG_FORMAT_LOG_DAY:
                replace(out, rep_str, fmt::format("{:0>2}", item.timestamp.day));
                break;
            case MKS_LOG_FORMAT_LOG_MONTH:
                replace(out, rep_str, fmt::format("{:0>2}", item.timestamp.month));
                break;
            case MKS_LOG_FORMAT_LOG_TIMESTAMP:
                replace(out, rep_str, item.timestamp.time);
                break;
            case MKS_LOG_FORMAT_LOG_PID:
                replace(out, rep_str, fmt::format("{}", item.pid));
                break;
            case MKS_LOG_FORMAT_LOG_TID: {
                auto tit = tid_2_str_map.find(item.tid);
                const std::string& tid_str =
                    tit != tid_2_str_map.end() ? tit->second : item.tid;
                replace(out, rep_str, tid_str);
                break;
            }
            case MKS_LOG_FORMAT_LOG_FILE:
                replace(out, rep_str, MKS_FILENAME(item.file));
                break;
            case MKS_LOG_FORMAT_LOG_LINE:
                replace(out, rep_str, fmt::format("{}", item.line));
                break;
            case MKS_LOG_FORMAT_LOG_FUNCTION:
                replace(out, rep_str, MKS_METHOD(item.function));
                break;
            case MKS_LOG_FORMAT_LOG_MESSAGE:
                replace(out, rep_str, item.message);
                break;
            case MKS_LOG_FORMAT_LOG_COLOR_START:
                if(enable_colors) {
                    replace(
                        out,
                        rep_str,
                        fmt::format(
                            "\033[{}m", mks::log::level_modifier(item.level).code()));
                } else {
                    replace(out, rep_str, "");
                }
                break;
            case MKS_LOG_FORMAT_LOG_COLOR_END:
                if(enable_colors) {
                    replace(out, rep_str, fmt::format("\033[{}m", mks::log::FG_DEFAULT));
                } else {
                    replace(out, rep_str, "");
                }
                break;
        }
    }
    return out;
}