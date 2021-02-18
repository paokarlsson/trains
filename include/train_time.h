/**
 * \author [Ola Karlsson](mailto:olka0600@student.miun.se)
 * \copyright Copyright 2020 Ola Karlsson. All rights reserved.
 */

#ifndef PROJECT_INCLUDE_TRAIN_TIME_H_
#define PROJECT_INCLUDE_TRAIN_TIME_H_

#include <math.h>

#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

/** \brief This is a class for converting time format to readable string.
* The constructor is private so it can not be instantiated normal way.
*/
class TrainTime {
 public:
  static std::string Time_tToString(time_t time_stamp) {
    time_t t = time_stamp + (60 * 60);  // Daylight saving hack
    std::tm t_struct{};
    gmtime_s(&t_struct, &t);
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << t_struct.tm_hour << ":"
        << std::setw(2) << std::setfill('0') << t_struct.tm_min;
    return oss.str();
  }
  static std::string SecondsToPretty(int seconds) {
    int sec = seconds % 60;
    int min = (seconds / 60) % 60;
    int hour = seconds / 3600;
    std::ostringstream oss;
    oss << std::setw((static_cast<int>(log10(hour)) + 1 < 2)
                         ? 2
                         : static_cast<int>(log10(hour)) + 1)
        << std::setfill('0') << hour << ":" << std::setw(2) << std::setfill('0')
        << min;
    return oss.str();
  }

 private:
  TrainTime() = default;
};
#endif  // PROJECT_INCLUDE_TRAIN_TIME_H_
