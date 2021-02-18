/**
 * \author [Ola Karlsson](mailto:olka0600@student.miun.se)
 * \copyright Copyright 2020 Ola Karlsson. All rights reserved.
 */

#include "simulator.h"  //NOLINT

#include <fstream>

#include "event.h"        //NOLINT
#include "station.h"      //NOLINT
#include "t_s_manager.h"  //NOLINT
#include "train.h"        //NOLINT
#include "train_map.h"    //NOLINT
#include "train_time.h"   //NOLINT
#include "vehicle.h"      //NOLINT

void Simulator::setupTime() {
  time_t now = time(nullptr);
  std::tm t_struct{};
  gmtime_s(&t_struct, &now);
  t_struct.tm_hour = 0;
  t_struct.tm_min = 0;
  t_struct.tm_sec = 0;
  SetCurrentTime(mktime(&t_struct));
  SetStartSimulationTime(GetCurrentTime());
  SetStopSimulationTime(GetCurrentTime() +
                        static_cast<time_t>((24 * 60 * 60) - 60));
  SetStopTime(GetStopSimulationTime());
  std::ifstream file("Trainsim.log");
  if (file.is_open()) {
    file.close();
    std::remove("Trainsim.log");
  }
}

void Simulator::AddEvent(const std::shared_ptr<Event> &event) {
  event_queue_.push(event);
}

void Simulator::AddToEventLog(const std::shared_ptr<Event> &event) {
  event_log_.emplace_back(event);
}

time_t Simulator::GetTime() const { return event_queue_.top()->GetEventTime(); }

bool Simulator::RunEventsUntilTime() {
  while (!event_queue_.empty() &&
         ((event_queue_.top()->GetEventTime() < GetCurrentTime()) ||
          (GetCurrentTime() >= GetStopSimulationTime()))) {
    RunNextEvent();
  }
  return !event_queue_.empty();
}

bool Simulator::RunNextEvent() {
  if (!event_queue_.empty()) {
    auto next_event = event_queue_.top();
    event_queue_.pop();
    if (!event_queue_.empty() &&
        event_queue_.top()->GetEventTime() < GetStopSimulationTime()) {
      next_event->Run();
    } else if (next_event->GetTrainStatus() == TrainStatus::RUNNING ||
               next_event->GetTrainStatus() == TrainStatus::ARRIVED) {
      next_event->Run();
      SetCurrentTime(next_event->GetEventTime());
    }
    return true;
  } else {
    return false;
  }
}

bool Simulator::GetTrainLifeCycleByTrainNumber(int train_number,
                                               std::string &details_out,
                                               bool high_detail_level) {
  std::ostringstream oss;
  int i = 0;
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
          if (high_detail_level) {
            auto connected_vehicles = event->GetState().connected_vehicles_;
            std::for_each(connected_vehicles.begin(), connected_vehicles.end(),
                          [&](int &vehicle_id) {
                            std::shared_ptr<Vehicle> vehicle_out;

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

bool Simulator::GetTrainLifeCycleByVehicleId(int vehicle_id,
                                             std::string &details_out,
                                             bool high_detail_level) {
  std::ostringstream oss;
  int i = 0;
  std::for_each(
      event_log_.begin(), event_log_.end(), [&](std::shared_ptr<Event> &event) {
        // if (event->GetTrain()->GetTrainNumber() == train_number) {
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
          if (high_detail_level) {
            auto connected_vehicles = event->GetState().connected_vehicles_;
            std::for_each(connected_vehicles.begin(), connected_vehicles.end(),
                          [&](int &vehicle_id) {
                            std::shared_ptr<Vehicle> vehicle_out;
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
