#ifndef FORMAT_H
#define FORMAT_H

#include <string>

namespace Format {
int constexpr MINUTE_SECONDS = 60;
int constexpr HOUR_SECONDS = 60 * MINUTE_SECONDS;
// Formats seconds count to HH:MM:SS string
std::string ElapsedTime(long seconds);
};  // namespace Format

#endif