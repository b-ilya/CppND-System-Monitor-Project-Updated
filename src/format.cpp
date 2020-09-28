#include "format.h"

#include <iomanip>
#include <sstream>
#include <string>

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  std::ostringstream result;
  result << std::setw(2) << std::setfill('0') << (seconds / HOUR_SECONDS);
  result << ":";
  result << std::setw(2) << std::setfill('0')
         << (seconds % HOUR_SECONDS / MINUTE_SECONDS);
  result << ":";
  result << std::setw(2) << std::setfill('0') << (seconds % MINUTE_SECONDS);
  return result.str();
}