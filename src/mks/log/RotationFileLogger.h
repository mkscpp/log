//
// Created by Michal Němec on 29.12.2020.
//

#ifndef MKS_ROTATIONFILELOGGER_H
#define MKS_ROTATIONFILELOGGER_H

#include <fstream>
#include <filesystem>

#include "../log.h"
#include "Logger.h"

namespace mks {

class RotationFileLogger : public BaseLogger {
    Logger logger_;
    const char* log_format_string_ = "%l%o%d %t %p %h [%f:%g] %s: %m\n";
    mks::LogFormat log_format_;
    std::ofstream ofs_;

    std::string base_filepath_;
    std::string file_path_meta_;
    std::string file_path_current_;
    std::size_t current_idx_ = 0;
    std::size_t current_size_ = 0;

    std::size_t max_size_ = 0;
    std::size_t num_files_ = 0;

    void write_current_idx_meta();
    void open_current_file(const std::string& filepath, std::ios_base::openmode mode);
    void next_file();

public:
    /**
     * @param filepath base path
     * @param max_size size of one file in bytes
     * @param num_files number of files that will be generated
     */
    RotationFileLogger(const std::string& filepath, std::size_t max_size, std::size_t num_files);
    void log(int level, int line, const char* file, const char* function, const char* tag, const char* message) override;
};

} // namespace mks

#endif // MKS_ROTATIONFILELOGGER_H
