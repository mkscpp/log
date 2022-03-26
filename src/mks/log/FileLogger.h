//
// Created by Michal Němec on 29.12.2020.
//

#ifndef MKS_FILELOGGER_H
#define MKS_FILELOGGER_H

#include <fstream>
#include <string>

#include "../log.h"
#include "Logger.h"

namespace mks {

class FileLogger : public BaseLogger {
    Logger logger;
    const char* log_format_string = "%l%o%d %t %p %h [%f:%g] %s: %m\n";
    mks::LogFormat log_format;
    std::ofstream ofs;

public:
    FileLogger();
    
    explicit FileLogger(const std::string& filepath, std::ios_base::openmode mode = std::ios_base::out);

    void create_file(const char* folder, const char* prefix = NULL);

    void log(int level, int line, const char* file, const char* function, const char* tag, const char* message) override;
};

} // namespace mks

#endif // MKS_FILELOGGER_H
