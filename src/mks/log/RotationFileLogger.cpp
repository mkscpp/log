//
// Created by Michal Němec on 29.12.2020.
//

#include "RotationFileLogger.h"

using namespace mks;

void RotationFileLogger::write_current_idx_meta() {
    std::ofstream meta_file(file_path_meta_);
    meta_file << current_idx_;
}

void RotationFileLogger::open_current_file(const std::string& filepath, std::ios_base::openmode mode) {
    if(ofs_.is_open()) {
        ofs_.close();
    }
    ofs_.open(filepath, mode);
    {
        std::streampos fsize = 0;
        std::ifstream file(filepath, std::ios::binary);
        fsize = file.tellg();
        file.seekg(0, std::ios::end);
        fsize = file.tellg() - fsize;
        current_size_ = fsize;
    }
}

void RotationFileLogger::next_file() {
    current_idx_ = (current_idx_ + 1) % num_files_;
    file_path_current_ = fmt::format("{}_{}.txt", base_filepath_, current_idx_);
    open_current_file(file_path_current_, std::ios_base::out);
    write_current_idx_meta();
}

RotationFileLogger::RotationFileLogger(const std::string& filepath, std::size_t max_size, std::size_t num_files)
    : base_filepath_(filepath), max_size_(max_size), num_files_(num_files) {

    // if we dont have files it doesn't make sense
    MKS_ASSERT(num_files > 0);

    // if we can't write anything also doesn't make sense
    MKS_ASSERT(num_files > 0);

    // we will use special meta file to store current logger index
    // other implementations might be renaming/moving files and write to the
    // same name this implementation will just jump between files
    file_path_meta_ = fmt::format("{}_meta.txt", filepath);
    if(std::filesystem::exists(file_path_meta_)) {
        std::ifstream file(file_path_meta_, std::ios::binary);
        if(!(file >> current_idx_)) {
            current_idx_ = 0;
            write_current_idx_meta();
        } else {
            // when we change number of files between runs
            if(current_idx_ >= num_files) {
                current_idx_ = 0;
                write_current_idx_meta();
            }
        }
    } else {
        current_idx_ = 0;
        write_current_idx_meta();
    }

    // we know current index of a files that was used last time
    file_path_current_ = fmt::format("{}_{}.txt", base_filepath_, current_idx_);
    open_current_file(file_path_current_, std::ios_base::app);

    // set up normal logger callback, called from log() method
    logger_.set_on_log_listener([this](LogItem& item) {
        // generate log line
        auto output = log_format_.format(log_format_string_, item);
        auto write_size = output.size();

        // REMARK: if we ever output more than max_size_ we cant ensure file size to be less than requested max_size_
        // if size overflows switch to the next file
        if(current_size_ + write_size > max_size_) {
            next_file();
        }

        // write it to the file itself
        ofs_.write(output.data(), write_size);

        // store current size
        // TODO: check if this can be done or we need to fetch actual size from tellg on the file
        current_size_ += write_size;
    });
}

void RotationFileLogger::log(int level, int line, const char* file, const char* function, const char* tag, const char* message) {
    // passthrough
    logger_.log(level, line, file, function, tag, message);
}