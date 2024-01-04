#ifndef FORMAT_H
#define FORMAT_H

#include <string>

namespace Format {
std::string Time(long time);  // TODO: See src/format.cpp
std::string ToMB(long &&kb);
};  // namespace Format

#endif