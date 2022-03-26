//
// Created by Michal Němec on 29.12.2020.
//

#include "ChainLogger.h"

using namespace mks;

void ChainLogger::log(int level, int line, const char* file, const char* function, const char* tag, const char* message) {
    for(auto& logger : loggers_) {
        if(logger == nullptr) {
            continue;
        }
        logger->log(level, line, file, function, tag, message);
    }
}