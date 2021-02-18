/**
 * \author [Ola Karlsson](mailto:olka0600@student.miun.se)
 * \copyright Copyright 2020 Ola Karlsson. All rights reserved.
 */

#ifndef PROJECT_INCLUDE_T_S_MANAGER_H_
#define PROJECT_INCLUDE_T_S_MANAGER_H_

#include <list>
#include <utility>
#include <memory>
#include <string>

class Distance;
class Simulator;
class Train;
class Station;
class Vehicle;

/** \brief This is holding the data used in the simulation. It holds a list
 * of all stations, trains and distances between stations. It also holds a
 * weak pointer to the simulator object.
 *
 */
class TrainStationManager
    : public std::enable_shared_from_this<TrainStationManager> {
  std::list<std::shared_ptr<Station>> stations_;
  std::list<std::shared_ptr<Train>> trains_;
  std::list<std::shared_ptr<Distance>> distances_;
  std::weak_ptr<Simulator> simulator_;
  bool high_log_level_vehicle_;
  bool high_log_level_station_;
  bool high_log_level_train_;
  bool high_log_level_stats_;

  /** \brief List of stations and how many vehicles i holds before the
   * simulation
   */
  std::list<std::pair<std::shared_ptr<Station>, int>>
      vehicle_distribution_start;

  /** \brief Loads station-/train-/distances- list with data from file specified
   * in the path parameter.. This function throws exception if file is not
   * found. The catch for this excepting is in the main.cpp file. */
  void loadStations(const std::string &path);
  void loadTrains(const std::string &path);
  void loadMap(const std::string &path);

  /** This function is populating the event queue with events. This is
   * basically the time table - 30 min. */
  void loadEvents();
  void setVehicleDistributionFromStart();

 public:
  TrainStationManager(std::shared_ptr<Simulator> simulator,
                      const std::string &station_path,
                      const std::string &trains_path,
                      const std::string &map_path);
  ~TrainStationManager() {}

  /** \brief This function is called after instansiation of this
   * TrainStationManager-object. This is because the funcion uses
   * share_from_this() to pass an instace of itself along to the events. And
   * this can not be made before instansiation is completed. */
  void Setup() { loadEvents(); }

  std::string SeeTimeTable();

  /** \brief 30 min before planed departure this function is called.
   * The function try to connect all demanded vehicles to the train using
   * the vehicle pool available on the station. */
  bool TryAssemble(int id);
  /** \brief After arrival this function is called.
   * The function disconnect all vehicles and return them to the station
   * vehicle pool.. */
  void DisAssemble(int id);

  /** \brief These funcions returns a station-/train-pointer. If name is spelled
   * wrong or if station/train do not exist it throws exception and depending on
   * were it was called from the exception is catched by the
   * calling function or by the main.cpp. */
  std::shared_ptr<Station> GetStationByName(const std::string &name);
  std::shared_ptr<Train> GetTrainByTrainNumber(int number);
  std::string GetTrainDetailsByTrainNumber(int train_number);
  std::string GetTrainDetailsByVehicleId(int vehicle_id);

  /** \brief Returns the distance between to stations. */
  int GetDistanceFrom(const std::string &station1, const std::string &station2);

  bool IsHighLogLevelTrain() const;
  void SetHighLogLevelTrain(bool high_log_level_train);
  bool IsHighLogLevelStation() const;
  void SetHighLogLevelStation(bool high_log_level_station);
  bool IsHighLogLevelVehicle() const;
  void SetHighLogLevelVehicle(bool high_log_level_vehicle);
  bool IsHighLogLevelStats() const;
  void SetHighLogLevelStats(bool high_log_level_stats);

  std::string ListAllStationNames();
  std::shared_ptr<Vehicle> FindVehicle(int id,
                                       std::string &location);  // NOLINT
  std::string GetVehicleDistributionStart();
  std::string GetTrainsStuckAtStation();
  std::string GetTrainsThatArrivedInTime();
  std::string GetDelayedTrains();
  bool GetTrainLifeCycleByTrainNumber(int train_number,
                                      std::string &details_out);  // NOLINT
  bool GetTrainLifeCycleByVehicleId(int vehicle_id,
                                    std::string &details_out);  // NOLINT

  std::string GetStationDetails(const std::string &name,
                                bool high_detail_level);
};

#endif  // PROJECT_INCLUDE_T_S_MANAGER_H_
