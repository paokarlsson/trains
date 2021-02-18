/**
 * \author [Ola Karlsson](mailto:olka0600@student.miun.se)
 * \copyright Copyright 2020 Ola Karlsson. All rights reserved.
 */

#ifndef PROJECT_INCLUDE_TRAIN_H_
#define PROJECT_INCLUDE_TRAIN_H_

#include <iosfwd>
#include <list>
#include <memory>
#include <string>
#include <vector>

class Station;
class Station;
class App;
class Vehicle;
class Event;

/** \brief This is an enum class used for train status.
 */
enum class TrainStatus {
  NOT_ASSEMBLED,
  INCOMPLETE,
  ASSEMBLED,
  READY,
  RUNNING,
  ARRIVED,
  FINISHED
};

std::ostream &operator<<(std::ostream &os, const TrainStatus &train_status);

/** \brief This class holds static information of each train line.
 * All class members are set from the constructor and accesseble through
 * getters.
 */
class TrainLine {
  const int id_;
  std::string departure_station_;
  std::string arrival_station_;
  const time_t departure_time_;
  const time_t arrival_time_;
  const std::vector<int> demanded_vehicles_;
  const int max_speed_;

 public:
  TrainLine(int max_speed, int id, const std::vector<int> &demanded_vehicles,
            std::string departure_station, std::string arrival_station,
            time_t departure_time, time_t arrival_time)
      : max_speed_(max_speed),
        id_(id),
        demanded_vehicles_(demanded_vehicles),
        departure_station_(departure_station),
        arrival_station_(arrival_station),
        departure_time_(departure_time),
        arrival_time_(arrival_time) {}
  ~TrainLine() {}

  int GetTrainNumber() const { return id_; }
  int GetMaxSpeed() const { return max_speed_; }
  std::vector<int> GetDemandedVehicles() const { return demanded_vehicles_; }
  std::string GetDepartureStation() { return departure_station_; }
  std::string GetArrivalStation() { return arrival_station_; }
  time_t GetDepartureTime() const { return departure_time_; }
  time_t GetArrivalTime() const { return arrival_time_; }
};

/** \brief This class represents a specific train and holds an instance of
 * a train line-object. The train-class also holds two list/vectors, one for
 * demanded vehicle types and one for vehicles that are connected to the train.
 *
 */
class Train {
  TrainLine train_line_;
  TrainStatus train_status_;
  std::vector<int> demanded_vehicles_;
  std::list<std::shared_ptr<Vehicle>> vehicles_;
  time_t planed_departure_time_;
  time_t expected_arrival_time_;

 public:
  explicit Train(const TrainLine &train_template)
      : train_line_(train_template),
        train_status_(TrainStatus::NOT_ASSEMBLED),
        vehicles_(0),
        planed_departure_time_(train_template.GetDepartureTime()),
        expected_arrival_time_(train_line_.GetArrivalTime()),
        demanded_vehicles_(train_template.GetDemandedVehicles()) {}
  ~Train() {}

  int GetTrainNumber() const { return train_line_.GetTrainNumber(); }
  TrainStatus GetTrainStatus() const { return train_status_; }
  void SetTrainStatus(TrainStatus train_status) {
    train_status_ = train_status;
  }

  std::string GetDepartureStation() {
    return train_line_.GetDepartureStation();
  }
  std::string GetArrivalStation() { return train_line_.GetArrivalStation(); }

  time_t GetOriginalDepartureTime() const {
    return train_line_.GetDepartureTime();
  }
  time_t GetOriginalArrivalTime() const { return train_line_.GetArrivalTime(); }

  /** \brief This function returns expected time of departure. This returns
   * the actual time of departure when all vehiclas are connected successfully.
   */
  time_t GetPlanedDepartureTime() const { return planed_departure_time_; }

  /** \brief This function returns the expected time of arrival. */
  time_t GetExpectedArrivalTime() const { return expected_arrival_time_; }

  void SetPlanedDepartureTime(time_t planed_departure_time) {
    planed_departure_time_ = planed_departure_time;
  }

  /** \brief This function is used for updating the expected time of arrival.
   * This function is awoken from within Event-class when the time of arrival
   * has been calcualted if train is delayed from station.
   */
  void SetExpectedArrivalTime(time_t expected_arrival_time) {
    expected_arrival_time_ = expected_arrival_time;
  }

  /** \brief Returns the max speed a specific train line can handle. */
  int GetTrainMaxSpeed() { return train_line_.GetMaxSpeed(); }

  /** \brief Returns the max speed a specific vehicle combination can handle. */
  int GetVehicleMaxSpeed();

  /** \brief This function returns a list of which vehicle types that is needed
   * for becoming complete.
   */
  std::string ListDemandedVehicles();
  /** \brief This function returns a list of which vehicle types that was needed
   * for becoming complete, at a specific time in history.
   */
  static std::string ListDemandedVehiclesFromState(
      std::vector<int> demanded_vehicles);
  std::string ListConnectedVehicles();

  std::string GetTimeTableData();
  std::string GetDataToLog(bool high_log_level);
  std::string GetDataToLogLow();
  std::string GetTrainDetails(bool high_detail);

  /** \brief This funcion is used when assembling a train */
  void AddVehicle(std::shared_ptr<Vehicle> &vehicle);  // NOLINT

  /** \brief This funcion is used when dissassembling a train */
  bool PopVehicle(std::shared_ptr<Vehicle> &vehicle_out);  // NOLINT

  bool GetVehicleById(int id, std::shared_ptr<Vehicle> &out_vehicle);  // NOLINT
  std::vector<int> &GetDemandedVehicles() { return demanded_vehicles_; }
  std::vector<int> GetConnectedVehicles();

  std::string GetLocation();
};

/** \brief This is a functor used for creating the time table. The priority
 * queue sorts the queue on planed departure time.
 */
class SortOnDepartureTime {
 public:
  bool operator()(std::weak_ptr<Train> first, std::weak_ptr<Train> second) {
    return first.lock()->GetPlanedDepartureTime() >
           second.lock()->GetPlanedDepartureTime();
  }
};

#endif  // PROJECT_INCLUDE_TRAIN_H_
