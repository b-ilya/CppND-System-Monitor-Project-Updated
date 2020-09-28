#ifndef FORMAT_H
#define FORMAT_H

#include <string>

namespace Format {
int constexpr MINUTE_SECONDS = 60;
int constexpr HOUR_SECONDS = 60 * MINUTE_SECONDS;
std::string ElapsedTime(long times);
};  // namespace Format

#endif