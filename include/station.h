/**
 * \author [Ola Karlsson](mailto:olka0600@student.miun.se)
 * \copyright Copyright 2020 Ola Karlsson. All rights reserved.
 */

#ifndef PROJECT_INCLUDE_STATION_H_
#define PROJECT_INCLUDE_STATION_H_

#include <iosfwd>
#include <list>
#include <memory>
#include <string>

class Train;
class Train;
class Vehicle;

/** \brief This class represent a train station.
 * It has a vehicle pool that the trains use assebling. 
 */
class Station {
  int id_;
  std::string name_;
  std::list<std::shared_ptr<Vehicle>> vehicle_pool_;

 public:
  Station(int id, std::string name);
  virtual ~Station() {}
  int GetId() const { return id_; }
  std::string GetName() const { return name_; }
  void AddToPool(std::shared_ptr<Vehicle> vehicle);
  bool GetVehicleByType(int type,
                        std::shared_ptr<Vehicle> &out_vehicle);        // NOLINT
  bool GetVehicleById(int id, std::shared_ptr<Vehicle> &out_vehicle);  // NOLINT
  int GetNumberOfVehicles();
};

bool operator==(std::shared_ptr<Station> &lhs, //NOLINT
                std::shared_ptr<Station> &rhs);  // NOLINT

#endif  // PROJECT_INCLUDE_STATION_H_
