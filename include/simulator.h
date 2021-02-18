/**
 * \author [Ola Karlsson](mailto:olka0600@student.miun.se)
 * \copyright Copyright 2020 Ola Karlsson. All rights reserved.
 */

#ifndef PROJECT_INCLUDE_SIMULATOR_H_
#define PROJECT_INCLUDE_SIMULATOR_H_

#include <iosfwd>
#include <list>
#include <memory>
#include <queue>
#include <string>
#include <vector>

#include "event.h"  //NOLINT

/** \brief This class is performing the simulation.
 * It holds all time stamps and also the event queue and the event log.
 */
class Simulator {
  time_t start_simulation_time_;
  time_t current_time_;
  time_t stop_simulation_time_;
  time_t stop_time_;
  int discrete_interval_;
  bool high_detail_level_;
  time_t total_delay;
  time_t total_departure_delay;
  std::priority_queue<std::shared_ptr<Event>,
                      std::vector<std::shared_ptr<Event>>, EventCompare>
      event_queue_;
  std::list<std::shared_ptr<Event>> event_log_;

  void setupTime();

 public:
  Simulator()
      : total_delay(0),
        total_departure_delay(0),
        high_detail_level_(0),
        discrete_interval_(10),
        current_time_(0),
        stop_time_(0),
        start_simulation_time_(0),
        stop_simulation_time_(0) {
    setupTime();
  }
  ~Simulator() {}

  void AddEvent(const std::shared_ptr<Event> &event);
  void AddToEventLog(const std::shared_ptr<Event> &event);

  /** Returns the time of the next comming event. */
  time_t GetTime() const;

  time_t GetCurrentTime() const { return current_time_; }

  /** This function pops event until event time >= current time. */
  bool RunEventsUntilTime();

  /** This function pops one event. */
  bool RunNextEvent();

  /** This function increment the total delay counter. */
  void AddToDelay(time_t sec) { total_delay += sec; }

  /** This function increment the total departure delay counter. */
  void AddToDepartureDelay(time_t sec) { total_departure_delay += sec; }
  time_t GetTotalDelay() { return total_delay; }
  time_t GetTotalDepartureDelay() { return total_departure_delay; }
  bool IsHighDetailLevel() { return high_detail_level_; }
  void SetHighDetailLevel(bool high_detail_level) {
    high_detail_level_ = high_detail_level;
  }
  time_t GetStopSimulationTime() const { return stop_simulation_time_; }
  time_t GetStartSimulationTime() const { return start_simulation_time_; }
  time_t GetStopTime() const { return stop_time_; }
  void SetCurrentTime(time_t current_time) { current_time_ = current_time; }
  void SetStartSimulationTime(time_t start_simulation_time) {
    start_simulation_time_ = start_simulation_time;
  }
  void SetStopSimulationTime(time_t stop_simulation_time) {
    stop_simulation_time_ = stop_simulation_time;
  }

  void SetStopTime(time_t stop_time) { stop_time_ = stop_time; }
  void SetDiscreteInterval(int discrete_interval) {
    discrete_interval_ = discrete_interval;
  }
  int GetDiscreteInterval() const { return discrete_interval_; }

  std::list<std::shared_ptr<Event>> GetEventLog() { return event_log_; }

  /** \brief This function returns the life cycle of a train by iterating over
   * the event log. */
  bool GetTrainLifeCycleByTrainNumber(int train_number,
                                      std::string &details_out,  // NOLINT
                                      bool high_detail_level);
  bool GetTrainLifeCycleByVehicleId(int vehicle_id,
                                    std::string &details_out,  // NOLINT
                                    bool high_detail_level);
};

#endif  // PROJECT_INCLUDE_SIMULATOR_H_
