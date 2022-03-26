//
// Created by Michal Němec on 30.01.2021.
//

#include "dump.h"

namespace mks {

std::ostream& dump_position(std::ostream& os, size_t c) {
    os << fmt::format("{:04x}: ", c);
    return os;
}

std::ostream& dump_byte(std::ostream& os, unsigned char c) {
    os << fmt::format("{:02x} ", c);
    return os;
}

std::ostream& dump_multi_bytes(std::ostream& os, unsigned char* c, size_t len) {
    for(size_t i = 0; i != len; ++i) {
        dump_byte(os, c[i]);
    }
    return os;
}

std::string dump(void* data, size_t size) {
    size_t i;
    size_t c;
    auto* ptr = (unsigned char*)data;
    unsigned int width = 0x10;
    std::ostringstream oss;
    for(i = 0; i < size; i += width) {
        oss << fmt::format("{:04x}: ", i);
        /* show hex to the left */
        for(c = 0; c < width; c++) {
            if(i + c < size) {
                oss << fmt::format("{:02x} ", ptr[i + c]);
            } else {
                oss << "   ";
            }
        }
        /* show data on the right */
        for(c = 0; (c < width) && (i + c < size); c++) {
            unsigned char x = (ptr[i + c] >= 0x20 && ptr[i + c] < 0x80) ?
                              (unsigned char)ptr[i + c] :
                              '.';
            oss << x;
        }
        if(i+width < size) oss << "\n";
    }
    return oss.str();
}

}