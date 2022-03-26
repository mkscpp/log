//
// Created by Michal Němec on 29.12.2020.
//

#include <mks/log.h>
#include <mks/log/ChainLogger.h>
#include <mks/log/Logger.h>
#include <mks/log/FileLogger.h>
#include <mks/log/RotationFileLogger.h>

int main() {
    mks::Logger normal_log;
    mks::FileLogger file_log("output.txt");
    mks::RotationFileLogger rot_file_log("output.txt", 1024*1024, 3);
    mks::ChainLogger chain_log;

    normal_log.set_on_log_listener([](mks::LogItem& item){
        mks::Logger::default_log(item);
    });

    // prepare chained logger
    chain_log
        .add(normal_log)
        .add(file_log)
        .add(rot_file_log);

    for(int i = 0; i<1000000; ++i) {
        MKS_LOG_LD(&chain_log, "Hello world {}", i);
    }

    return 0;
}