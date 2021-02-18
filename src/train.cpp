/**
 * \author [Ola Karlsson](mailto:olka0600@student.miun.se)
 * \copyright Copyright 2020 Ola Karlsson. All rights reserved.
 */

#include "train.h"  //NOLINT

#include <algorithm>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>

#include "train_time.h"  //NOLINT
#include "vehicle.h"     //NOLINT

std::ostream &operator<<(std::ostream &os, const TrainStatus &train_status) {
  switch (train_status) {
    case TrainStatus::NOT_ASSEMBLED:
      os << "Not assembled";
      break;
    case TrainStatus::INCOMPLETE:
      os << "Incomplete";
      break;
    case TrainStatus::ASSEMBLED:
      os << "Assembled";
      break;
    case TrainStatus::READY:
      os << "Ready";
      break;
    case TrainStatus::RUNNING:
      os << "Running";
      break;
    case TrainStatus::ARRIVED:
      os << "Arrived";
      break;
    case TrainStatus::FINISHED:
      os << "Finished";
      break;
  }
  return os;
}

int Train::GetVehicleMaxSpeed() {
  int max_speed = -1;
  std::for_each(
      vehicles_.begin(), vehicles_.end(),
      [&](std::shared_ptr<Vehicle> &vehicle) {
        if (vehicle->GetType() == 4 || vehicle->GetType() == 5) {
          int new_max_speed =
              std::dynamic_pointer_cast<Locomotive>(vehicle)->GetMaxSpeed();
          if (max_speed == -1) {
            max_speed = new_max_speed;
          } else if (max_speed > new_max_speed) {
            max_speed = new_max_speed;
          }
        }
      });
  return max_speed;
}

std::string Train::GetTimeTableData() {
  std::ostringstream oss;
  oss << std::left << std::setw(6)
      << TrainTime::Time_tToString(planed_departure_time_) << std::left
      << std::setw(10)
      << std::string(
             "(" + TrainTime::Time_tToString(GetOriginalDepartureTime()) + ")")
      << std::setw(8) << GetTrainNumber() << std::setw(20)
      << GetDepartureStation() << std::left << std::setw(6)
      << TrainTime::Time_tToString(expected_arrival_time_) << std::setw(10)
      << std::string("(" + TrainTime::Time_tToString(GetOriginalArrivalTime()) +
                     ")")
      << std::setw(20) << GetArrivalStation();
  if (planed_departure_time_ != GetOriginalDepartureTime() ||
      expected_arrival_time_ != GetOriginalArrivalTime()) {
    oss << "+"
        << TrainTime::SecondsToPretty(static_cast<int>(
               expected_arrival_time_ - GetOriginalArrivalTime()));
  }
  return oss.str();
}

std::string Train::GetDataToLog(bool high_log_level) {
  std::ostringstream oss;
  if (high_log_level) {
    oss << GetDataToLogLow() << "\n"
        << ListDemandedVehicles() << "\n"
        << ListConnectedVehicles() << "\n";
  } else {
    oss << GetDataToLogLow();
  }
  return oss.str();
}

std::string Train::GetDataToLogLow() {
  std::ostringstream oss;
  oss << "Train: " << GetTrainNumber() << " from " << GetDepartureStation()
      << " " << TrainTime::Time_tToString(GetPlanedDepartureTime()) << " ("
      << TrainTime::Time_tToString(GetOriginalDepartureTime()) << ") to "
      << GetArrivalStation() << " "
      << TrainTime::Time_tToString(GetExpectedArrivalTime()) << " ("
      << TrainTime::Time_tToString(GetOriginalArrivalTime())
      << ") Train status: " << GetTrainStatus();
  return oss.str();
}

std::string Train::GetTrainDetails(bool high_detail) {
  std::ostringstream oss;
  if (high_detail) {
    oss << GetTimeTableData() << "\n";
    if (!vehicles_.empty()) oss << ListConnectedVehicles();
    if (!demanded_vehicles_.empty()) oss << ListDemandedVehicles();
  } else {
    oss << GetTimeTableData();
  }
  return oss.str();
}

std::string Train::ListConnectedVehicles() {
  std::ostringstream oss;
  if (!vehicles_.empty()) {
    oss << "Connected vehicles:\n";
    std::for_each(vehicles_.begin(), vehicles_.end(),
                  [&](std::shared_ptr<Vehicle> &vehicle) {
                    oss << vehicle->GetDetails() << "\n";
                  });
    oss << "\n";
  }
  return oss.str();
}

std::string Train::ListDemandedVehicles() {
  std::ostringstream oss;
  oss << ListDemandedVehiclesFromState(demanded_vehicles_);
  return oss.str();
}

std::string Train::ListDemandedVehiclesFromState(
    std::vector<int> demanded_vehicles) {
  std::ostringstream oss;
  if (!demanded_vehicles.empty()) {
    oss << "Demanded vehicles:\n";
    int coach = 0, sleeping = 0, open = 0, covered = 0, electrical = 0,
        diesel = 0;
    std::for_each(demanded_vehicles.begin(), demanded_vehicles.end(),
                  [&](int &type) {
                    switch (type) {
                      case 0:
                        coach++;
                        break;
                      case 1:
                        sleeping++;
                        break;
                      case 2:
                        open++;
                        break;
                      case 3:
                        covered++;
                        break;
                      case 4:
                        electrical++;
                        break;
                      case 5:
                        diesel++;
                        break;
                      default:
                        break;
                    }
                  });
    if (electrical > 0) oss << electrical << " Electrical engine\n";
    if (diesel > 0) oss << diesel << " Diesel engine\n";
    if (coach > 0) oss << coach << " Coach car\n";
    if (sleeping > 0) oss << sleeping << " Sleeping car\n";
    if (open > 0) oss << open << " Open car\n";
    if (covered > 0) oss << covered << " Covered car\n";
  }
  return oss.str();
}

std::string Train::GetLocation() {
  std::ostringstream oss;
  oss << "Connected to train: " << GetTrainNumber();
  if (train_status_ == TrainStatus::INCOMPLETE ||
      train_status_ == TrainStatus::ASSEMBLED ||
      train_status_ == TrainStatus::NOT_ASSEMBLED ||
      train_status_ == TrainStatus::READY) {
    oss << " at " << GetDepartureStation();
  } else if (train_status_ == TrainStatus::ARRIVED ||
             train_status_ == TrainStatus::FINISHED) {
    oss << " at " << GetArrivalStation();
  } else {
    oss << " running to " << GetArrivalStation();
  }
  return oss.str();
}

void Train::AddVehicle(std::shared_ptr<Vehicle> &vehicle) {
  vehicles_.emplace_back(std::move(vehicle));
}

bool Train::PopVehicle(std::shared_ptr<Vehicle> &vehicle_out) {
  if (!vehicles_.empty()) {
    vehicle_out = std::move(*(vehicles_.begin()));
    vehicles_.erase(vehicles_.begin());
    return true;
  }
  return false;
}

bool Train::GetVehicleById(int id, std::shared_ptr<Vehicle> &out_vehicle) {
  auto it = find_if(vehicles_.begin(), vehicles_.end(),
                    [&id](std::shared_ptr<Vehicle> &vehicle) {
                      return vehicle->GetId() == id;
                    });
  if (it != vehicles_.end()) {
    out_vehicle = *it;
    return true;
  }
  return false;
}

std::vector<int> Train::GetConnectedVehicles() {
  std::vector<int> connected_vehicles;
  std::for_each(vehicles_.begin(), vehicles_.end(),
                [&connected_vehicles](std::shared_ptr<Vehicle> &vehicle) {
                  connected_vehicles.emplace_back(vehicle->GetId());
                });
  return connected_vehicles;
}
