/**
 * \author [Ola Karlsson](mailto:olka0600@student.miun.se)
 * \copyright Copyright 2020 Ola Karlsson. All rights reserved.
 */

#include "event.h" // NOLINT

#include <fstream>
#include <iostream>

#include "simulator.h"  // NOLINT
#include "t_s_manager.h" // NOLINT
#include "train.h" // NOLINT
#include "train_time.h" // NOLINT

void Event::Log() {
  state.planed_departure_time_ = train_->GetPlanedDepartureTime();
  state.expected_arrival_time_ = train_->GetExpectedArrivalTime();
  state.average_speed_ = getAverageSpeed();
  state.train_status_ = train_->GetTrainStatus();
  state.connected_vehicles_ = train_->GetConnectedVehicles();
  state.demanded_vehicles_ = train_->GetDemandedVehicles();
  simulator_.lock()->AddToEventLog(shared_from_this());

  if (event_time_ >= simulator_.lock()->GetStartSimulationTime()) {
    std::ostringstream oss;
    if (simulator_.lock()->IsHighDetailLevel()) {
      oss << "Event time: " << TrainTime::Time_tToString(event_time_) << "\n"
          << train_->GetDataToLog(true)
          << " Average speed: " << getAverageSpeed() << " km/h\n";
    } else {
      oss << TrainTime::Time_tToString(event_time_) << " "
          << train_->GetDataToLog(false) << "\n";
    }
    std::string log = oss.str();
    std::ofstream file("Trainsim.log", std::fstream::out | std::fstream::app);
    if (file.is_open()) {
      file << oss.str();
      file.close();
    }
    std::cout << log;
  }
}

void NotAssembled::Run() {
  if (train_station_environment_.lock()->TryAssemble(
          train_->GetTrainNumber())) {
    train_->SetTrainStatus(TrainStatus::ASSEMBLED);

    std::shared_ptr<Event> event = std::make_shared<Ready>(
        Ready(train_station_environment_.lock(), simulator_.lock(), train_,
              event_time_ + (20 * 60)));
    simulator_.lock()->AddEvent(event);
  } else {
    train_->SetPlanedDepartureTime(train_->GetPlanedDepartureTime() +
                                   (10 * 60));
    train_->SetTrainStatus(TrainStatus::INCOMPLETE);

    std::shared_ptr<Event> event = std::make_shared<Incomplete>(
        Incomplete(train_station_environment_.lock(), simulator_.lock(), train_,
                   event_time_ + (10 * 60)));
    simulator_.lock()->AddEvent(event);
  }
  Log();
}

void Incomplete::Run() {
  int potential_duration_s = getPotentialDuration();
  int original_duration_s = static_cast<int>(
      train_->GetOriginalArrivalTime() - train_->GetOriginalDepartureTime());
  time_t potential_arrival_time =
      train_->GetPlanedDepartureTime() + potential_duration_s;

  if (train_station_environment_.lock()->TryAssemble(
          train_->GetTrainNumber())) {
    if (potential_arrival_time > train_->GetOriginalArrivalTime()) {
      train_->SetExpectedArrivalTime(potential_arrival_time);
    } else {
      train_->SetExpectedArrivalTime(train_->GetPlanedDepartureTime() +
                                     original_duration_s);
    }
    train_->SetTrainStatus(TrainStatus::ASSEMBLED);
    std::shared_ptr<Event> event = std::make_shared<Ready>(
        Ready(train_station_environment_.lock(), simulator_.lock(), train_,
              event_time_ + (20 * 60)));
    simulator_.lock()->AddEvent(event);
  } else {
    train_->SetPlanedDepartureTime(train_->GetPlanedDepartureTime() +
                                   (10 * 60));

    if (potential_arrival_time >= train_->GetOriginalArrivalTime()) {
      train_->SetExpectedArrivalTime(potential_arrival_time + (10 * 60));
    } else {
      train_->SetExpectedArrivalTime(train_->GetPlanedDepartureTime() +
                                     original_duration_s + (10 * 60));
    }
    train_->SetTrainStatus(TrainStatus::INCOMPLETE);
    std::shared_ptr<Event> event = std::make_shared<Incomplete>(
        Incomplete(train_station_environment_.lock(), simulator_.lock(), train_,
                   event_time_ + (10 * 60)));
    simulator_.lock()->AddEvent(event);
  }
  Log();
}

int Incomplete::getAccDist_Meter_Second(int max_speed, float acceleration) {
  int avg_speed = static_cast<int>(static_cast<float>(max_speed) / 2);
  return static_cast<int>(static_cast<float>(avg_speed) *
                          static_cast<float>(avg_speed) / acceleration);
}
int Incomplete::getPotentialDuration() {
  int max_speed = train_->GetTrainMaxSpeed();
  if (train_->GetTrainStatus() != TrainStatus::NOT_ASSEMBLED &&
      train_->GetTrainStatus() != TrainStatus::INCOMPLETE) {
    int challenger = train_->GetVehicleMaxSpeed();
    if (challenger < max_speed) max_speed = challenger;
  }
  int max_speed_m_s = static_cast<int>(static_cast<float>(max_speed) / 3.6);

  int distance_m =
      1000 * train_station_environment_.lock()->GetDistanceFrom(
                 train_->GetDepartureStation(), train_->GetArrivalStation());
  int acc_time = getAccDist_Meter_Second(max_speed_m_s, acceleration);
  int dec_time = getAccDist_Meter_Second(max_speed_m_s, deceleration);
  int dist_full_speed = distance_m - acc_time + dec_time;
  return (2 * acc_time + 2 * dec_time + dist_full_speed) / max_speed_m_s;
}

void Ready::Run() {
  train_->SetTrainStatus(TrainStatus::READY);
  std::shared_ptr<Event> event = std::make_shared<Running>(
      Running(train_station_environment_.lock(), simulator_.lock(), train_,
              event_time_ + (10 * 60)));
  simulator_.lock()->AddEvent(event);
  Log();
}

void Running::Run() {
  train_->SetTrainStatus(TrainStatus::RUNNING);
  if (train_->GetOriginalDepartureTime() != train_->GetPlanedDepartureTime()) {
    simulator_.lock()->AddToDepartureDelay(train_->GetPlanedDepartureTime() -
                                           train_->GetOriginalDepartureTime());
  }
  std::shared_ptr<Event> event = std::make_shared<Arrived>(
      train_station_environment_.lock(), simulator_.lock(), train_,
      train_->GetExpectedArrivalTime());
  simulator_.lock()->AddEvent(event);
  Log();
}

int Running::getAverageSpeed() {
  int duration =
      train_->GetExpectedArrivalTime() - train_->GetPlanedDepartureTime();
  int distance_m =
      1000 * train_station_environment_.lock()->GetDistanceFrom(
                 train_->GetDepartureStation(), train_->GetArrivalStation());
  float m_per_s = static_cast<float>(distance_m) / static_cast<float>(duration);
  return static_cast<int>(m_per_s * 3.6);
}

void Arrived::Run() {
  train_->SetTrainStatus(TrainStatus::ARRIVED);
  if (train_->GetOriginalArrivalTime() != train_->GetExpectedArrivalTime()) {
    simulator_.lock()->AddToDelay(train_->GetExpectedArrivalTime() -
                                  train_->GetOriginalArrivalTime());
  }
  std::shared_ptr<Event> event = std::make_shared<Finished>(
      Finished(train_station_environment_.lock(), simulator_.lock(), train_,
               event_time_ + (20 * 60)));
  simulator_.lock()->AddEvent(event);
  Log();
}

void Finished::Run() {
  train_station_environment_.lock()->DisAssemble(train_->GetTrainNumber());
  train_->SetTrainStatus(TrainStatus::FINISHED);
  Log();
}

TrainStatus Event::GetTrainStatus() { return train_->GetTrainStatus(); }
std::shared_ptr<Train> &Event::GetTrain() { return train_; }
bool EventCompare::operator()(const std::shared_ptr<Event> &first,
                              const std::shared_ptr<Event> &second) {
  return first->GetEventTime() > second->GetEventTime();
}
