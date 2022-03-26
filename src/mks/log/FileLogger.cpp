//
// Created by Michal Němec on 29.12.2020.
//

#include "FileLogger.h"

using namespace mks;

FileLogger::FileLogger() {
    logger.set_on_log_listener([this](LogItem& item) {
        auto output = log_format.format(log_format_string, item);
        ofs.write(output.data(), output.size());
    });
}

FileLogger::FileLogger(const std::string& filepath, std::ios_base::openmode mode) {
    ofs.open(filepath, mode);
    logger.set_on_log_listener([this](LogItem& item) {
        auto output = log_format.format(log_format_string, item);
        ofs.write(output.data(), output.size());
    });
}

void FileLogger::log(int level, int line, const char* file, const char* function, const char* tag, const char* message) {
    logger.log(level, line, file, function, tag, message);
}

void FileLogger::create_file(const char* folder, const char* prefix) {
//    char path[PATH_MAX];
    char ts[32];
    std::string path;

//    sprintf(path, "%s/log/", OLD_APP_ROOT);
    path = folder;

    if(prefix) {
        path += prefix;
        path += "_";
    }

    // format timestamp
    struct tm localTime{};
    time_t t = time(nullptr);
    localtime_r(&t, &localTime);
    strftime(ts, sizeof(ts), "%Y-%m-%d_%H-%M-%S", &localTime);

    path += ts;
    path += ".log";
    fprintf(stderr, "Opening log file %s\n", path.c_str());

/*
    if(interactiveMode) {
        char cmd[1024];
        sprintf(cmd, "tee %s", path.c_str());
        log_file = popen(cmd, "w");
    } else
*/

    ofs.open(path, std::ios_base::out);
    logger.set_on_log_listener([this](LogItem& item) {
        auto output = log_format.format(log_format_string, item);
        ofs.write(output.data(), output.size());
    });
}
