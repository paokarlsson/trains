/**
 * \author [Ola Karlsson](mailto:olka0600@student.miun.se)
 * \copyright Copyright 2020 Ola Karlsson. All rights reserved.
 */

#ifndef PROJECT_INCLUDE_VEHICLE_H_
#define PROJECT_INCLUDE_VEHICLE_H_

#include <iosfwd>
#include <memory>
#include <string>

/** \brief This is the Vehicle base class.
 * This class has two virtual functions GetType and GetDetatils.
 */
class Vehicle {
  int id_;

 public:
  explicit Vehicle(int id);
  virtual ~Vehicle();
  int GetId() const { return id_; }
  void SetId(int id) { id_ = id; }
  virtual const int GetType() const = 0;
  virtual std::string GetDetails() = 0;
  std::string GetDetailsLow();
};

class Locomotive : public Vehicle {
  int max_speed_;

 public:
  Locomotive(int _id, int _maxSpeed) : Vehicle(_id), max_speed_(_maxSpeed) {}
  virtual ~Locomotive() {}
  int GetMaxSpeed() const { return max_speed_; }
  const int GetType() const override = 0;
  std::string GetDetails() override = 0;
};

class Diesel : public Locomotive {
  int fuel_consumption_;
  const int TYPE;

 public:
  Diesel(int id, int max_speed, int fuel_consumption)
      : Locomotive(id, max_speed),
        TYPE(5),
        fuel_consumption_(fuel_consumption) {}
  virtual ~Diesel() {}
  int GetFuelConsumption() const { return fuel_consumption_; }
  const int GetType() const override { return TYPE; }
  std::string GetDetails() override;
};

class Electrical : public Locomotive {
  int maxPower;
  const int TYPE;

 public:
  Electrical(int id, int max_speed, int max_power)
      : Locomotive(id, max_speed), TYPE(4), maxPower(max_power) {}
  virtual ~Electrical() {}
  int GetMaxPower() const { return maxPower; }
  const int GetType() const override { return TYPE; }
  std::string GetDetails() override;
};

class Carriage : public Vehicle {
 public:
  explicit Carriage(int id) : Vehicle(id) {}
  virtual ~Carriage() {}
  const int GetType() const override = 0;
  std::string GetDetails() override = 0;
};

class PassengerCar : public Carriage {
 public:
  explicit PassengerCar(int id) : Carriage(id) {}
  virtual ~PassengerCar() {}
  const int GetType() const override = 0;
  std::string GetDetails() override = 0;
};

class CoachCar : public PassengerCar {
  int number_of_chairs_;
  bool has_internet_;
  const int TYPE;

 public:
  CoachCar(int id, int number_of_chairs, bool has_internet)
      : PassengerCar(id),
        TYPE(0),
        number_of_chairs_(number_of_chairs),
        has_internet_(has_internet) {}
  virtual ~CoachCar() {}
  int GetNumberOfChairs() const { return number_of_chairs_; }
  void SetNumberOfChairs(int number_of_chairs) {
    number_of_chairs_ = number_of_chairs;
  }
  bool IsHasInternet() const { return has_internet_; }
  const int GetType() const override { return TYPE; }
  std::string GetDetails() override;
};

class SleepingCar : public PassengerCar {
  int number_of_beds_;
  const int TYPE;

 public:
  SleepingCar(int id, int number_of_beds)
      : PassengerCar(id), number_of_beds_(number_of_beds), TYPE(1) {}
  virtual ~SleepingCar() {}
  int GetNumberOfBeds() const { return number_of_beds_; }
  const int GetType() const override { return TYPE; }
  std::string GetDetails() override;
};

class FreightCar : public Carriage {
 public:
  explicit FreightCar(int id) : Carriage(id) {}
  virtual ~FreightCar() {}
  const int GetType() const override = 0;
  std::string GetDetails() override = 0;
};

class OpenCar : public FreightCar {
  int weight_capacity_;
  int floor_area_;
  const int TYPE;

 public:
  OpenCar(int id, int weight_capacity, int floor_area)
      : FreightCar(id),
        weight_capacity_(weight_capacity),
        floor_area_(floor_area),
        TYPE(2) {}
  virtual ~OpenCar() {}
  int GetWeightCapacity() const { return weight_capacity_; }
  int GetFloorArea() const { return floor_area_; }
  const int GetType() const override { return TYPE; }
  std::string GetDetails() override;
};

class CoveredCar : public FreightCar {
  int volume_capacity_;
  const int TYPE;

 public:
  CoveredCar(int id, int volume_capacity)
      : FreightCar(id), volume_capacity_(volume_capacity), TYPE(3) {}
  virtual ~CoveredCar() {}
  int GetVolumeCapacity() const { return volume_capacity_; }
  const int GetType() const override { return TYPE; }
  std::string GetDetails() override;
};

#endif  // PROJECT_INCLUDE_VEHICLE_H_
