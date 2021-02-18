/**
 * \author [Ola Karlsson](mailto:olka0600@student.miun.se)
 * \copyright Copyright 2020 Ola Karlsson. All rights reserved.
 */

#include "station.h"  //NOLINT

#include <algorithm>
#include <fstream>
#include <string>
#include <utility>

#include "vehicle.h"  //NOLINT

Station::Station(int id, std::string name) : id_(id), name_(std::move(name)) {}

void Station::AddToPool(std::shared_ptr<Vehicle> vehicle) {
  vehicle_pool_.emplace_back(std::move(vehicle));
}

bool Station::GetVehicleByType(int type,
                               std::shared_ptr<Vehicle> &out_vehicle) {
  auto it = find_if(vehicle_pool_.begin(), vehicle_pool_.end(),
                    [&type, &out_vehicle](std::shared_ptr<Vehicle> &vehicle) {
                      if (vehicle->GetType() == type) {
                        out_vehicle = std::move(vehicle);
                        return true;
                      }
                      return false;
                    });

  if (it != vehicle_pool_.end()) {
    vehicle_pool_.erase(it);
    return true;
  }
  return false;
}

bool Station::GetVehicleById(int id,
                             std::shared_ptr<Vehicle> &out_vehicle) {  // NOLINT
  auto it = find_if(vehicle_pool_.begin(), vehicle_pool_.end(),
                    [&id](std::shared_ptr<Vehicle> &vehicle) {
                      return vehicle->GetId() == id;
                    });
  if (it != vehicle_pool_.end()) {
    out_vehicle = *it;
    return true;
  }
  return false;
}

int Station::GetNumberOfVehicles() { return vehicle_pool_.size(); }

bool operator==(std::shared_ptr<Station> &lhs, //NOLINT
                std::shared_ptr<Station> &rhs) {  // NOLINT
  return lhs->GetId() == rhs->GetId();
}
