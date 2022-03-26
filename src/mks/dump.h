//
// Created by Michal Němec on 06/08/2020.
//

#ifndef MKS_DUMP_H
#define MKS_DUMP_H

#include <fmt/format.h>
#include <sstream>
#include <string>

namespace mks {

std::ostream& dump_position(std::ostream& os, size_t c);
std::ostream& dump_byte(std::ostream& os, unsigned char c);
std::ostream& dump_multi_bytes(std::ostream& os, unsigned char* c, size_t len);
std::string dump(void* data, size_t size);

} // namespace mks

#endif // MKS_DUMP_H
