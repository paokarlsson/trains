/**
 * \author [Ola Karlsson](mailto:olka0600@student.miun.se)
 * \copyright Copyright 2020 Ola Karlsson. All rights reserved.
 */

#ifndef PROJECT_INCLUDE_TRAIN_MAP_H_
#define PROJECT_INCLUDE_TRAIN_MAP_H_

#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

/** \brief This class holds two train stations and the distance between them. 
 */
class Distance {
 private:
  std::string station_1_;
  std::string station_2_;
  int distance_;

 public:
  Distance(const std::string &station_1, const std::string &station_2,
           int distance)
      : station_1_(station_1), station_2_(station_2), distance_(distance) {}
  virtual ~Distance() {}
  std::string GetStation1() { return station_1_; }
  std::string GetStation2() { return station_2_; }
  int GetDistance() const { return distance_; }
  void SetDistance(int distance) { Distance::distance_ = distance; }
};

#endif  // PROJECT_INCLUDE_TRAIN_MAP_H_
