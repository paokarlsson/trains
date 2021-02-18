/**
 * \author [Ola Karlsson](mailto:olka0600@student.miun.se)
 * \copyright Copyright 2020 Ola Karlsson. All rights reserved.
 */

#include "vehicle.h"  //NOLINT

#include <iomanip>
#include <sstream>

Vehicle::Vehicle(int id) : id_(id) {}
Vehicle::~Vehicle() {}

std::string Vehicle::GetDetailsLow() {
  std::ostringstream oss;
  oss << "Id: " << GetId() << " Type: " << GetType();
  return oss.str();
}

std::string Diesel::GetDetails() {
  std::ostringstream oss;
  oss << " Id: " << GetId() << " Diesel "
      << " Max speed (km/h): " << GetMaxSpeed()
      << " Fuel consumption (l/h): " << GetFuelConsumption();
  return oss.str();
}

std::string Electrical::GetDetails() {
  std::ostringstream oss;
  oss << " Id: " << GetId() << " Electrical "
      << " Max speed (km/h): " << GetMaxSpeed()
      << " Max power (kW): " << GetMaxPower();
  return oss.str();
}

std::string CoachCar::GetDetails() {
  std::ostringstream oss;
  oss << "Id: " << GetId() << " Coach car"
      << " Number of seats: " << GetNumberOfChairs()
      << " Has internet: " << ((IsHasInternet()) ? "Yes" : "No");
  return oss.str();
}

std::string SleepingCar::GetDetails() {
  std::ostringstream oss;
  oss << " Id: " << GetId() << " Sleeping car "
      << " Number of beds: " << GetNumberOfBeds();
  return oss.str();
}

std::string OpenCar::GetDetails() {
  std::ostringstream oss;
  oss << " Id: " << GetId() << " Open car "
      << " Weight capacity (ton): " << GetWeightCapacity()
      << " Floor area (m2): " << GetFloorArea();
  return oss.str();
}

std::string CoveredCar::GetDetails() {
  std::ostringstream oss;
  oss << " Id: " << GetId() << " Covered car "
      << " Volume capacity (m3): " << GetVolumeCapacity();
  return oss.str();
}
