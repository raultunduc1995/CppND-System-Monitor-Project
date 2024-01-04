#include "format.h"

#include <iomanip>
#include <sstream>
#include <string>

using std::string;
using std::to_string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::Time(long int seconds) {
  long int minutes{0};
  long int hours{0};

  minutes = seconds / 60;
  seconds %= 60;
  hours = minutes / 60;
  minutes %= 60;

  std::stringstream ss;
  ss << std::setfill('0') << std::setw(2) << hours << ":" << std::setw(2)
     << minutes << ":" << std::setw(2) << seconds;

  return ss.str();
}

string Format::ToMB(long &&kb) {
  float mb = kb / 1024.0;

  std::stringstream ss;
  ss << std::fixed << std::setprecision(0) << mb;

  return ss.str();
}