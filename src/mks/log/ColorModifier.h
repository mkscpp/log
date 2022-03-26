//
// Created by Michal Němec on 03/02/2020.
//

#ifndef MKS_COLORMODIFIER_H
#define MKS_COLORMODIFIER_H

#include <ostream>

namespace mks::log {

enum Code {
    FG_RED = 31,
    FG_GREEN = 32,
    FG_BLUE = 34,
    FG_MAGENTA = 35,
    FG_DEFAULT = 39,
    BG_RED = 41,
    BG_GREEN = 42,
    BG_BLUE = 44,
    BG_DEFAULT = 49
};

class Modifier {
    Code code_ = FG_DEFAULT;

public:
    Modifier(Code pCode) : code_(pCode) {}
    Modifier(const Modifier& mod) = default;
    int code() const {
        return code_;
    }
    friend std::ostream& operator<<(std::ostream& os, const Modifier& mod) {
        return os << "\033[" << mod.code_ << "m";
    }
};

} // namespace mks

#endif // MKS_COLORMODIFIER_H
