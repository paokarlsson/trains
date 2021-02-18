/**
 * \author [Ola Karlsson](mailto:olka0600@student.miun.se)
 * \copyright Copyright 2020 Ola Karlsson. All rights reserved.
 */

#ifndef PROJECT_INCLUDE_EVENT_H_
#define PROJECT_INCLUDE_EVENT_H_

#include <iosfwd>
#include <memory>
#include <vector>

class Simulator;
class Train;
enum class TrainStatus;
class TrainStationManager;

/** \brief Holds the state of a event.
 * Each event that has passed is saved to a list called event log. The event
 * contains a train object pointer. This public class saves the all the states
 * that can vary between different events.
 */
class State {
 public:
  TrainStatus train_status_;
  time_t planed_departure_time_;
  time_t expected_arrival_time_;
  int average_speed_;
  std::vector<int> connected_vehicles_;
  std::vector<int> demanded_vehicles_;
};

/** \brief This is the Event base-class.
 */
class Event : public std::enable_shared_from_this<Event> {
 protected:
  State state;
  std::weak_ptr<TrainStationManager> train_station_environment_;
  std::weak_ptr<Simulator> simulator_;
  std::shared_ptr<Train> train_;
  time_t event_time_;
  virtual int getAverageSpeed() = 0;

 public:
  explicit Event(std::shared_ptr<TrainStationManager> train_station_environment,
                 std::shared_ptr<Simulator> simulator,
                 std::shared_ptr<Train> train, time_t event_time)
      : train_station_environment_(train_station_environment),
        simulator_(simulator),
        train_(train),
        event_time_(event_time) {}
  virtual ~Event() {}

  time_t GetEventTime() const { return event_time_; }
  virtual void Run() = 0;
  TrainStatus GetTrainStatus();
  void Log();
  std::shared_ptr<Train> &GetTrain();

  State GetState() { return state; }
};

class NotAssembled : public Event {
  int getAverageSpeed() { return 0; }

 public:
  NotAssembled(std::shared_ptr<TrainStationManager> train_station_environment,
               std::shared_ptr<Simulator> simulator,
               std::shared_ptr<Train> train, time_t event_time)
      : Event(train_station_environment, simulator, train, event_time) {}
  virtual ~NotAssembled() {}
  void Run() override;
};

/** \brief This event-class is used when NotAssembled class fail to asseble
 * a train. The class calculates new arrival time based on max speed on
 * train line or vehicle set. One asumption have been made that the train
 * accelerate with 0.2 mps2 until the train reaches max speed and hold this
 * speed is until deceleration begins with the same amount, 0.2, before reaching
 * destination. This might not be the perfectly realistic model but its a model.
 */
class Incomplete : public Event {
  static int getAccDist_Meter_Second(int max_speed, float acceleration);
  int getPotentialDuration();
  int getAverageSpeed() { return 0; }
  float acceleration;
  float deceleration;

 public:
  Incomplete(std::shared_ptr<TrainStationManager> train_station_environment,
             std::shared_ptr<Simulator> simulator, std::shared_ptr<Train> train,
             time_t event_time)
      : Event(train_station_environment, simulator, train, event_time),
        acceleration(0.2f),
        deceleration(0.2f) {}
  virtual ~Incomplete() {}
  void Run() override;
};

class Ready : public Event {
  int getAverageSpeed() { return 0; }

 public:
  Ready(std::shared_ptr<TrainStationManager> train_station_environment,
        std::shared_ptr<Simulator> simulator, std::shared_ptr<Train> train,
        time_t event_time)
      : Event(train_station_environment, simulator, train, event_time) {}
  virtual ~Ready() {}
  void Run() override;
};

class Running : public Event {
  int getAverageSpeed();

 public:
  Running(std::shared_ptr<TrainStationManager> train_station_environment,
          std::shared_ptr<Simulator> simulator, std::shared_ptr<Train> train,
          time_t event_time)
      : Event(train_station_environment, simulator, train, event_time) {}
  virtual ~Running() {}
  void Run() override;
};

class Arrived : public Event {
  int getAverageSpeed() { return 0; }

 public:
  Arrived(std::shared_ptr<TrainStationManager> train_station_environment,
          std::shared_ptr<Simulator> simulator, std::shared_ptr<Train> train,
          time_t event_time)
      : Event(train_station_environment, simulator, train, event_time) {}
  virtual ~Arrived() {}
  void Run() override;
};

class Finished : public Event {
  int getAverageSpeed() { return 0; }

 public:
  Finished(std::shared_ptr<TrainStationManager> train_station_environment,
           std::shared_ptr<Simulator> simulator, std::shared_ptr<Train> train,
           time_t event_time)
      : Event(train_station_environment, simulator, train, event_time) {}
  virtual ~Finished() {}
  void Run() override;
};

class EventCompare {
 public:
  bool operator()(const std::shared_ptr<Event> &first,
                  const std::shared_ptr<Event> &second);
};

#endif  // PROJECT_INCLUDE_EVENT_H_
