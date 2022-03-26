//
// Created by Michal Němec on 29.12.2020.
//

#ifndef MKS_CHAINLOGGER_H
#define MKS_CHAINLOGGER_H

#include "../log.h"

namespace mks {

class ChainLogger : public BaseLogger {
    std::vector<BaseLogger*> loggers_;

public:
    void log(int level, int line, const char* file, const char* function, const char* tag, const char* message) override;
    template <typename T>
    ChainLogger& add(T& next_logger) {
        loggers_.emplace_back(static_cast<BaseLogger*>(&next_logger));
        return *this;
    }
};

} // namespace mks

#endif // MKS_CHAINLOGGER_H
