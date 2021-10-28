/**
 * \author [Ola Karlsson](mailto:olka0600@student.miun.se)
 * \copyright Copyright 2020 Ola Karlsson. All rights reserved.
 */

#include "t_s_manager.h" //NOLINT

#include <algorithm>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <memory>
#include <queue>
#include <vector>

#include "simulator.h" //NOLINT
#include "station.h" //NOLINT
#include "train.h" //NOLINT
#include "train_map.h" //NOLINT
#include "train_time.h" //NOLINT
#include "vehicle.h" //NOLINT

TrainStationManager::TrainStationManager(std::shared_ptr<Simulator> simulator,
                                         const std::string &station_path,
                                         const std::string &trains_path,
                                         const std::string &map_path)
    : simulator_(simulator),
      high_log_level_station_(false),
      high_log_level_train_(false),
      high_log_level_stats_(false),
      high_log_level_vehicle_(false) {
  loadStations(station_path);
  loadTrains(trains_path);
  loadMap(map_path);
  setVehicleDistributionFromStart();
}

void TrainStationManager::loadStations(const std::string &path) {
  std::ifstream file(path);
  if (file.is_open()) {
    int station_id = 1;
    while (file.peek() != EOF) {
      std::string line;
      std::getline(file, line);
      std::istringstream iss(line);
      std::string name;
      iss >> name;
      auto station = std::make_shared<Station>(station_id, name);
      station_id++;
      while (iss.peek() != EOF) {
        std::string dummy, line1;
        std::getline(iss, dummy, '(');
        std::getline(iss, line1, ')');
        std::istringstream isss(line1);
        int id, type, param_0, param_1;
        isss >> id >> type >> param_0;
        if (type == 0) {
          isss >> param_1;
          station->AddToPool(std::make_shared<CoachCar>(id, param_0, param_1));
        } else if (type == 1) {
          station->AddToPool(std::make_shared<SleepingCar>(id, param_0));
        } else if (type == 2) {
          isss >> param_1;
          station->AddToPool(std::make_shared<OpenCar>(id, param_0, param_1));
        } else if (type == 3) {
          station->AddToPool(std::make_shared<CoveredCar>(id, param_0));
        } else if (type == 4) {
          isss >> param_1;
          station->AddToPool(
              std::make_shared<Electrical>(id, param_0, param_1));
        } else if (type == 5) {
          isss >> param_1;
          station->AddToPool(std::make_shared<Diesel>(id, param_0, param_1));
        }
      }
      stations_.emplace_back(station);
    }
    file.close();
  } else {
    throw std::runtime_error("Could not open file " + path);
  }
}

void TrainStationManager::loadTrains(const std::string &path) {
  std::ifstream file(path);
  if (file.is_open()) {
    while (file.peek() != EOF) {
      std::string line;
      std::getline(file, line);
      std::istringstream iss(line);
      std::string dep_st, arr_st;
      std::tm time1{}, time2{};
      int id, max_speed;
      iss >> id >> dep_st >> arr_st >> std::get_time(&time1, "%H:%M") >>
          std::get_time(&time2, "%H:%M") >> max_speed;
      time_t today = time(nullptr);
      std::tm todayStruct{};
      gmtime_r(&today,&todayStruct);
      todayStruct.tm_hour = time1.tm_hour;
      todayStruct.tm_min = time1.tm_min;
      todayStruct.tm_sec = 0;
      time_t departure_time = mktime(&todayStruct);
      todayStruct.tm_hour = time2.tm_hour;
      todayStruct.tm_min = time2.tm_min;
      time_t arrival_time = mktime(&todayStruct);
      std::vector<int> vehicle_types;
      while (iss.peek() != EOF) {
        int type;
        iss >> type;
        vehicle_types.emplace_back(type);
      }
      TrainLine train_template(max_speed, id, vehicle_types, dep_st, arr_st,
                               departure_time, arrival_time);
      trains_.emplace_back(std::make_shared<Train>(train_template));
    }
    file.close();
  } else {
    throw std::runtime_error("Could not open file " + path);
  }
}

void TrainStationManager::loadMap(const std::string &path) {
  std::ifstream file(path);
  if (file.is_open()) {
    while (file.peek() != EOF) {
      std::string line;
      std::getline(file, line);
      std::istringstream iss(line);
      std::string st_1, st_2;
      int dist;
      iss >> st_1 >> st_2 >> dist;
      distances_.emplace_back(std::make_shared<Distance>(st_1, st_2, dist));
    }
    file.close();
  } else {
    throw std::runtime_error("Could not open file " + path);
  }
}

void TrainStationManager::loadEvents() {
  if (!trains_.empty()) {
    std::for_each(
        trains_.begin(), trains_.end(), [&](std::shared_ptr<Train> &train) {
          train->SetTrainStatus(TrainStatus::NOT_ASSEMBLED);
          train->SetPlanedDepartureTime(train->GetPlanedDepartureTime());
          std::shared_ptr<Event> e = std::make_shared<NotAssembled>(
              NotAssembled(shared_from_this(), simulator_.lock(), train,
                           train->GetOriginalDepartureTime() - (30 * 60)));
          simulator_.lock()->AddEvent(e);
        });
  } else {
    throw std::runtime_error("Trains list is empty!");
  }
}

std::string TrainStationManager::SeeTimeTable() {
  std::priority_queue<std::shared_ptr<Train>,
                      std::vector<std::shared_ptr<Train>>, SortOnDepartureTime>
      train_pri_list;
  std::for_each(
      trains_.begin(), trains_.end(),
      [&](std::shared_ptr<Train> &train) { train_pri_list.push(train); });
  std::ostringstream oss;
  oss << std::left << std::setw(16) << "Time" << std::setw(8) << "Number"
      << std::setw(20) << "Origin" << std::setw(16) << "Time" << std::setw(20)
      << "Destination"
      << "Delay\n";
  while (!train_pri_list.empty()) {
    oss << train_pri_list.top()->GetTimeTableData() << "\n";
    train_pri_list.pop();
  }
  if (simulator_.lock()->GetTotalDelay() > 0) {
    oss << std::setw(80) << "Total delay so far:"
        << std::string("+" + TrainTime::SecondsToPretty(static_cast<int>(
                                 simulator_.lock()->GetTotalDelay())))
        << "\n";
  }
  if (simulator_.lock()->GetTotalDepartureDelay() > 0) {
    oss << std::setw(80) << "Total departure delay so far:"
        << std::string("+" + TrainTime::SecondsToPretty(static_cast<int>(
                                 simulator_.lock()->GetTotalDepartureDelay())));
  }
  return oss.str();
}

bool TrainStationManager::TryAssemble(int id) {
  std::shared_ptr<Train> train = GetTrainByTrainNumber(id);
  int size = train->GetDemandedVehicles().size();
  std::shared_ptr<Station> station =
      GetStationByName(train->GetDepartureStation());
  int index = 0;
  while (size != 0 && index < size) {
    std::shared_ptr<Vehicle> vehicle;
    if (station->GetVehicleByType(
            *(train->GetDemandedVehicles().begin() + index), vehicle)) {
      train->GetDemandedVehicles().erase(train->GetDemandedVehicles().begin() +
                                         index);
      train->AddVehicle(vehicle);
      size--;
    } else {
      index++;
    }
  }
  if (size == 0)
    return true;
  else
    return false;
}

void TrainStationManager::DisAssemble(int id) {
  std::shared_ptr<Train> train = GetTrainByTrainNumber(id);
  std::shared_ptr<Station> station =
      GetStationByName(train->GetArrivalStation());
  std::shared_ptr<Vehicle> vehicle_out;
  while (train->PopVehicle(vehicle_out)) {
    station->AddToPool(std::move(vehicle_out));
  }
}

std::shared_ptr<Station> TrainStationManager::GetStationByName(
    const std::string &name) {
  auto it = std::find_if(stations_.begin(), stations_.end(),
                         [&name](std::shared_ptr<Station> &station) {
                           return station->GetName() == name;
                         });
  if (it != stations_.end()) {
    return *it;
  } else {
    throw std::runtime_error("There is no station with that name");
  }
}

std::shared_ptr<Train> TrainStationManager::GetTrainByTrainNumber(int number) {
  auto it = std::find_if(trains_.begin(), trains_.end(),
                         [number](std::shared_ptr<Train> &train) {
                           if (train->GetTrainNumber() == number) {
                             return true;
                           } else {
                             return false;
                           }
                         });
  if (it != trains_.end()) {
    return *it;
  } else {
    throw std::runtime_error("There is no train with that name");
  }
}

bool TrainStationManager::IsHighLogLevelTrain() const {
  return high_log_level_train_;
}

void TrainStationManager::SetHighLogLevelTrain(bool high_log_level_train) {
  high_log_level_train_ = high_log_level_train;
}

bool TrainStationManager::IsHighLogLevelStation() const {
  return high_log_level_station_;
}

void TrainStationManager::SetHighLogLevelStation(bool high_log_level_station) {
  high_log_level_station_ = high_log_level_station;
}

bool TrainStationManager::IsHighLogLevelVehicle() const {
  return high_log_level_vehicle_;
}

void TrainStationManager::SetHighLogLevelVehicle(bool high_log_level_vehicle) {
  high_log_level_vehicle_ = high_log_level_vehicle;
}

bool TrainStationManager::IsHighLogLevelStats() const {
  return high_log_level_stats_;
}

void TrainStationManager::SetHighLogLevelStats(bool high_log_level_stats) {
  high_log_level_stats_ = high_log_level_stats;
}

std::string TrainStationManager::GetTrainDetailsByTrainNumber(
    int train_number) {
  std::string details;
  auto it = std::find_if(
      trains_.begin(), trains_.end(), [&](std::shared_ptr<Train> &train) {
        if (train->GetTrainNumber() == train_number) {
          details = train->GetTrainDetails(IsHighLogLevelTrain());
          return true;
        } else {
          return false;
        }
      });
  if (it != trains_.end()) {
    return details;
  } else {
    throw std::runtime_error("There is no train with this number");
  }
}

std::string TrainStationManager::ListAllStationNames() {
  std::ostringstream oss;
  oss << "Names:\n";
  std::for_each(stations_.begin(), stations_.end(),
                [&oss](std::shared_ptr<Station> &station) {
                  oss << station->GetName() << "\n";
                });
  return oss.str();
}

std::shared_ptr<Vehicle> TrainStationManager::FindVehicle(
    int id, std::string &location) {
  std::shared_ptr<Vehicle> vehicle_out;
  auto it1 = std::find_if(
      stations_.begin(), stations_.end(),
      [&](std::shared_ptr<Station> &station) {
        location = std::string(" Parked at: " + station->GetName());
        return station->GetVehicleById(id, vehicle_out);
      });
  if (it1 != stations_.end()) {
    return vehicle_out;
  }
  auto it2 = std::find_if(
      trains_.begin(), trains_.end(), [&](std::shared_ptr<Train> &train) {
        std::ostringstream oss;
        oss << " connected to train: " << train->GetTrainNumber();
        location = oss.str();
        return train->GetVehicleById(id, vehicle_out);
      });
  if (it2 != trains_.end()) {
    return vehicle_out;
  } else {
    throw std::runtime_error("This vehicle id does not exists");
  }
}

bool TrainStationManager::GetTrainLifeCycleByTrainNumber(
    int train_number, std::string &details_out) {
  std::ostringstream oss;
  int i = 0;
  std::list<std::shared_ptr<Event>> event_log_ =
      simulator_.lock()->GetEventLog();
  std::for_each(
      event_log_.begin(), event_log_.end(), [&](std::shared_ptr<Event> &event) {
        if (event->GetTrain()->GetTrainNumber() == train_number) {
          oss << TrainTime::Time_tToString(event->GetEventTime()) << " "
              << "Train: " << event->GetTrain()->GetTrainNumber() << " from "
              << event->GetTrain()->GetDepartureStation() << " "
              << TrainTime::Time_tToString(
                     event->GetState().planed_departure_time_)
              << " ("
              << TrainTime::Time_tToString(
                     event->GetTrain()->GetOriginalDepartureTime())
              << ") to " << event->GetTrain()->GetArrivalStation() << " "
              << TrainTime::Time_tToString(
                     event->GetState().planed_departure_time_)
              << " ("
              << TrainTime::Time_tToString(
                     event->GetTrain()->GetOriginalArrivalTime())
              << ") Train status: " << event->GetState().train_status_ << "\n";
          if (high_log_level_stats_) {
            auto connected_vehicles = event->GetState().connected_vehicles_;
            std::for_each(connected_vehicles.begin(), connected_vehicles.end(),
                          [&](int &vehicle_id) {
                            std::string location;
                            std::shared_ptr<Vehicle> vehicle_out =
                                FindVehicle(vehicle_id, location);
                            event->GetTrain()->GetVehicleById(vehicle_id,
                                                              vehicle_out);
                            oss << vehicle_out->GetDetails() << "\n";
                          });
            oss << event->GetTrain()->ListDemandedVehiclesFromState(
                       event->GetState().demanded_vehicles_)
                << "\n";
          }
          i++;
        }
      });
  details_out = oss.str();
  return i != 0;
}

bool TrainStationManager::GetTrainLifeCycleByVehicleId(
    int vehicle_id, std::string &details_out) {
  std::ostringstream oss;
  int i = 0;
  std::list<std::shared_ptr<Event>> event_log_ =
      simulator_.lock()->GetEventLog();
  std::for_each(
      event_log_.begin(), event_log_.end(), [&](std::shared_ptr<Event> &event) {
        auto vehicles = event->GetState().connected_vehicles_;
        auto hit = std::find_if(vehicles.begin(), vehicles.end(),
                                [&](int id) { return vehicle_id == id; });
        if (hit != vehicles.end()) {
          oss << TrainTime::Time_tToString(event->GetEventTime()) << " "
              << "Train: " << event->GetTrain()->GetTrainNumber() << " from "
              << event->GetTrain()->GetDepartureStation() << " "
              << TrainTime::Time_tToString(
                     event->GetState().planed_departure_time_)
              << " ("
              << TrainTime::Time_tToString(
                     event->GetTrain()->GetOriginalDepartureTime())
              << ") to " << event->GetTrain()->GetArrivalStation() << " "
              << TrainTime::Time_tToString(
                     event->GetState().planed_departure_time_)
              << " ("
              << TrainTime::Time_tToString(
                     event->GetTrain()->GetOriginalArrivalTime())
              << ") Train status: " << event->GetState().train_status_ << "\n";
          if (high_log_level_stats_) {
            auto connected_vehicles = event->GetState().connected_vehicles_;
            std::for_each(connected_vehicles.begin(), connected_vehicles.end(),
                          [&](int &vehicle_id) {
                            std::string location;
                            std::shared_ptr<Vehicle> vehicle_out =
                                FindVehicle(vehicle_id, location);
                            event->GetTrain()->GetVehicleById(vehicle_id,
                                                              vehicle_out);
                            oss << vehicle_out->GetDetails() << "\n";
                          });
            oss << event->GetTrain()->ListDemandedVehiclesFromState(
                       event->GetState().demanded_vehicles_)
                << "\n";
          }
          i++;
        }
      });
  details_out = oss.str();
  return i != 0;
}

std::string TrainStationManager::GetVehicleDistributionStart() {
  std::ostringstream oss;
  std::for_each(
      vehicle_distribution_start.begin(), vehicle_distribution_start.end(),
      [&oss](std::pair<std::shared_ptr<Station>, int> &pair) {
        oss << std::left << std::setw(20)
            << std::string(pair.first->GetName() + ":") << pair.second << "\n";
      });
  return oss.str();
}

void TrainStationManager::setVehicleDistributionFromStart() {
  std::for_each(stations_.begin(), stations_.end(),
                [&](std::shared_ptr<Station> &station) {
                  vehicle_distribution_start.emplace_back(
                      std::make_pair(station, station->GetNumberOfVehicles()));
                });
}

std::string TrainStationManager::GetTrainsStuckAtStation() {
  std::ostringstream oss;
  std::for_each(trains_.begin(), trains_.end(),
                [&](std::shared_ptr<Train> &train) {
                  if (train->GetTrainStatus() == TrainStatus::INCOMPLETE) {
                    oss << train->GetTrainDetails(high_log_level_stats_);
                  }
                });
  return oss.str();
}

std::string TrainStationManager::GetTrainsThatArrivedInTime() {
  std::ostringstream oss;
  std::for_each(
      trains_.begin(), trains_.end(), [&](std::shared_ptr<Train> &train) {
        if (train->GetTrainStatus() == TrainStatus::FINISHED &&
            train->GetExpectedArrivalTime() ==
                train->GetOriginalArrivalTime()) {
          oss << train->GetTrainDetails(high_log_level_stats_) << "\n";
        }
      });
  return oss.str();
}

std::string TrainStationManager::GetDelayedTrains() {
  std::ostringstream oss;
  std::for_each(
      trains_.begin(), trains_.end(), [&](std::shared_ptr<Train> &train) {
        if (train->GetExpectedArrivalTime() > train->GetOriginalArrivalTime() &&
            train->GetTrainStatus() != TrainStatus::INCOMPLETE) {
          oss << train->GetTrainDetails(high_log_level_stats_) << "\n";
        }
      });
  return oss.str();
}

std::string TrainStationManager::GetStationDetails(const std::string &name,
                                                   bool high_detail_level) {
  std::ostringstream oss;
  std::shared_ptr<Station> station_out = GetStationByName(name);
  oss << station_out->GetName() << "\n\n"
      << "Train:\n";
  std::for_each(trains_.begin(), trains_.end(),
                [&](std::shared_ptr<Train> &train) {
                  if (train->GetDepartureStation() == station_out->GetName()) {
                    oss << train->GetTrainDetails(high_detail_level) << "\n";
                  }
                });
  if (high_detail_level) {
    oss << "Available vehicles:\n";
    // oss << station_out->PrintVehiclePool();
  }

  return oss.str();
}

std::string TrainStationManager::GetTrainDetailsByVehicleId(int vehicle_id) {
  std::string details;
  auto it = std::find_if(
      trains_.begin(), trains_.end(), [&](std::shared_ptr<Train> &train) {
        std::shared_ptr<Vehicle> vehicle_out;
        if (train->GetVehicleById(vehicle_id, vehicle_out)) {
          details = train->GetTrainDetails(IsHighLogLevelTrain());
          return true;
        } else {
          return false;
        }
      });
  if (it != trains_.end()) {
    return details;
  } else {
    throw std::runtime_error("Could not find a train containing this vehicle.");
  }
}

int TrainStationManager::GetDistanceFrom(const std::string &station1,
                                         const std::string &station2) {
  auto it =
      find_if(distances_.begin(), distances_.end(),
              [&station1, &station2](std::shared_ptr<Distance> &distance) {
                if ((station1 == distance->GetStation1() &&
                     station2 == distance->GetStation2()) ||
                    (station1 == distance->GetStation2() &&
                     station2 == distance->GetStation1())) {
                  return true;
                } else {
                  return false;
                }
              });
  return (*it)->GetDistance();
}
