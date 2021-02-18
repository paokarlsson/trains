/**
 * \author [Ola Karlsson](mailto:olka0600@student.miun.se)
 * \copyright Copyright 2020 Ola Karlsson. All rights reserved.
 */

#ifndef PROJECT_INCLUDE_APP_H_
#define PROJECT_INCLUDE_APP_H_

#include <iosfwd>
#include <list>
#include <memory>
#include <string>

#include "menu.h" //NOLINT

class Simulator;
class TrainStationManager;

/** \brief This is the user interface class.
 * It has one public funcion called Run. This is where the program starts for
 * the user. Here is the main menu presented.
 *
 */
class App {
  std::shared_ptr<Simulator> simulator;
  std::shared_ptr<TrainStationManager> train_station_manager;
  bool simulation_done_;

  Menu main_menu;
  Menu simulation_menu;
  Menu train_menu;
  Menu station_menu;
  Menu vehicle_menu;
  Menu statistics_menu;

  /** \brief Creates all menus. */
  void initMenus();

  /** \brief Sub menus. */
  void simulationMenu();
  void trainDetailsMenu();
  void stationDetailsMenu();
  void vehicleDetailsMenu();
  /** \brief This menu is not available before the simulation is done. */
  void statisticsMenu();

  /** All actions in this class is stored in a separte void function.
   * This functions is used when creating the menu items. */
  void seeTimeTable();
  void changeStartTime();
  void changeStopTime();
  void changeDiscreteInterval();
  void nextInterval();
  void nextEvent();
  void finishSimulation();
  void changeDetailLevel();
  void searchTrainByTrainNumber();
  void searchTrainByVehicleId();
  void changeTrainDetailLevel();
  void listAllStationNames();
  void searchStationByName();
  void changeStationDetailLevel();
  void searchVehicleById();
  void changeVehicleDetailLevel();
  void showAllStatistics();
  void showVehicleDistributionStart();
  void showTotalDelay();
  void showTrainsStuckAtStation();
  void showDelayedTrains();
  void showTrainsInTime();
  void showTrainLifeCycle();
  void showTrainLifeCycleByVehicleId();
  void changeStatsDetailLevel();
  void processEventsIfTime();
  static std::string getStringInput(const std::string &prompt);
  time_t inputTime();
  int inputInterval();
  static bool keyPressNotEnter();
  void setSimulationDone(bool isDone) { simulation_done_ = isDone; }
  bool isSimulationDone() { return simulation_done_; }
  static void removeOldLog();

 public:
  App(const std::string &ts_path, const std::string &t_path,
      const std::string &tm_path);
  ~App();
  void Run();
};

#endif  // PROJECT_INCLUDE_APP_H_
