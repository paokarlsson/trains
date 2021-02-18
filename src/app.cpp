/**
 * \author [Ola Karlsson](mailto:olka0600@student.miun.se)
 * \copyright Copyright 2020 Ola Karlsson. All rights reserved.
 */

#include "app.h"  //NOLINT

#include <ctime>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "menu.h"         //NOLINT
#include "simulator.h"    //NOLINT
#include "station.h"      //NOLINT
#include "t_s_manager.h"  //NOLINT
#include "train.h"        //NOLINT
#include "train_map.h"    //NOLINT
#include "train_time.h"   //NOLINT
#include "vehicle.h"      //NOLINT

App::App(const std::string &ts_path, const std::string &t_path,
         const std::string &tm_path)
    : simulation_done_(false),
      main_menu(Menu("Train simulator menu", true)),
      simulation_menu(Menu("Simulation controller", false)),
      train_menu(Menu("Train menu", false)),
      station_menu(Menu("Station menu", false)),
      vehicle_menu(Menu("Vehicle menu", false)),
      statistics_menu(Menu("Statistics menu", false)),
      simulator(std::make_shared<Simulator>()) {
  train_station_manager = std::make_shared<TrainStationManager>(
      simulator, ts_path, t_path, tm_path);
  removeOldLog();
  initMenus();
}
App::~App() {}

void App::initMenus() {
  MenuItem mm1("Simulation controller", true, [this]() { simulationMenu(); });
  MenuItem mm2("Time table", true, [this]() { seeTimeTable(); });
  MenuItem mm3("Train menu", true, [this]() { trainDetailsMenu(); });
  MenuItem mm4("Station menu", true, [this]() { stationDetailsMenu(); });
  MenuItem mm5("Vehicle menu", true, [this]() { vehicleDetailsMenu(); });
  main_menu.AddMenuItem(mm1);
  main_menu.AddMenuItem(mm2);
  main_menu.AddMenuItem(mm3);
  main_menu.AddMenuItem(mm4);
  main_menu.AddMenuItem(mm5);

  MenuItem sim1("Change start time", true, [this]() { changeStartTime(); });
  MenuItem sim2("Change stop time", true, [this]() { changeStopTime(); });
  MenuItem sim3("Change interval", true,
                [this]() { changeDiscreteInterval(); });
  MenuItem sim4("Next interval", true, [this]() { nextInterval(); });
  MenuItem sim5("Next event", true, [this]() { nextEvent(); });
  MenuItem sim6("Finish simulation", true, [this]() { finishSimulation(); });
  MenuItem sim7("Change detail level", true, [this]() { changeDetailLevel(); });
  MenuItem sim8("Statistics menu", false, [this]() { statisticsMenu(); });
  simulation_menu.AddMenuItem(sim1);
  simulation_menu.AddMenuItem(sim2);
  simulation_menu.AddMenuItem(sim3);
  simulation_menu.AddMenuItem(sim4);
  simulation_menu.AddMenuItem(sim5);
  simulation_menu.AddMenuItem(sim6);
  simulation_menu.AddMenuItem(sim7);
  simulation_menu.AddMenuItem(sim8);

  MenuItem tm1("Search by train number", true,
               [this]() { searchTrainByTrainNumber(); });
  MenuItem tm2("Search by vehicle id", true,
               [this]() { searchTrainByVehicleId(); });
  MenuItem tm3("Change detail level", true,
               [this]() { changeTrainDetailLevel(); });
  train_menu.AddMenuItem(tm1);
  train_menu.AddMenuItem(tm2);
  train_menu.AddMenuItem(tm3);

  MenuItem stm1("Show station names", true,
                [this]() { listAllStationNames(); });
  MenuItem stm2("Search station by name", true,
                [this]() { searchStationByName(); });
  MenuItem stm3("Change detail level", true,
                [this]() { changeStationDetailLevel(); });
  station_menu.AddMenuItem(stm1);
  station_menu.AddMenuItem(stm2);
  station_menu.AddMenuItem(stm3);

  MenuItem vm1("Search vehicle by id", true, [this]() { searchVehicleById(); });
  MenuItem vm2("Change detail level", true,
               [this]() { changeVehicleDetailLevel(); });
  vehicle_menu.AddMenuItem(vm1);
  vehicle_menu.AddMenuItem(vm2);

  MenuItem sm1("Show all statistics", true, [this]() { showAllStatistics(); });
  MenuItem sm2("Show number of vehicles at start of simulation", true,
               [this]() { showVehicleDistributionStart(); });
  MenuItem sm3("Trains that arrived in time", true,
               [this]() { showTrainsInTime(); });
  MenuItem sm4("See total delay", true, [this]() { showTotalDelay(); });
  MenuItem sm5("List of delayed trains", true,
               [this]() { showDelayedTrains(); });
  MenuItem sm6("List of trains that never left station", true,
               [this]() { showTrainsStuckAtStation(); });
  MenuItem sm7("A trains lifecycle, train number", true,
               [this]() { showTrainLifeCycle(); });
  MenuItem sm8("A trains lifecycle, vehicle id", true,
               [this]() { showTrainLifeCycleByVehicleId(); });
  MenuItem sm9("Change detail level", true,
               [this]() { changeStatsDetailLevel(); });
  statistics_menu.AddMenuItem(sm1);
  statistics_menu.AddMenuItem(sm2);
  statistics_menu.AddMenuItem(sm3);
  statistics_menu.AddMenuItem(sm4);
  statistics_menu.AddMenuItem(sm5);
  statistics_menu.AddMenuItem(sm6);
  statistics_menu.AddMenuItem(sm7);
  statistics_menu.AddMenuItem(sm8);
  statistics_menu.AddMenuItem(sm9);
}

void App::Run() {
  // Has to be done post instantiation. "share_from_this()"
  train_station_manager->Setup();

  do {
    main_menu.PrintMenu();
  } while (main_menu.DoChoice());
}

void App::simulationMenu() {
  do {
    simulation_menu.PrintMenu();
  } while (simulation_menu.DoChoice());
}

void App::trainDetailsMenu() {
  do {
    train_menu.PrintMenu();
  } while (train_menu.DoChoice());
}

void App::stationDetailsMenu() {
  do {
    station_menu.PrintMenu();
  } while (station_menu.DoChoice());
}

void App::vehicleDetailsMenu() {
  do {
    vehicle_menu.PrintMenu();
  } while (vehicle_menu.DoChoice());
}

void App::statisticsMenu() {
  do {
    statistics_menu.PrintMenu();
  } while (statistics_menu.DoChoice());
}

void App::seeTimeTable() {
  std::cout << train_station_manager->SeeTimeTable() << "\n";
}

void App::changeStartTime() {
  time_t temp;
  do {
    temp = inputTime();
    if (temp > simulator->GetStopTime()) {
      std::cout << temp << "\n";
      std::cout << simulator->GetStopTime() << "\n";
    }
  } while (temp > simulator->GetStopTime());
  simulator->SetCurrentTime(temp);
  simulator->SetStartSimulationTime(temp);
}

void App::changeStopTime() {
  time_t temp;
  do {
    temp = inputTime();
  } while (temp < simulator->GetCurrentTime());
  simulator->SetStopSimulationTime(temp);
}

void App::changeDiscreteInterval() {
  simulator->SetDiscreteInterval(inputInterval());
}

void App::nextInterval() {
  do {
    processEventsIfTime();
    if (keyPressNotEnter()) break;
    simulator->SetCurrentTime(
        simulator->GetCurrentTime() +
        static_cast<time_t>(simulator->GetDiscreteInterval()) *
            static_cast<time_t>(60));
  } while (true);
  simulation_menu.SetMenuItemEnabled("Statistics menu", true);
}

void App::nextEvent() {
  simulator->SetCurrentTime(simulator->GetTime());
  if (!simulator->RunNextEvent()) {
    setSimulationDone(true);
    simulation_menu.SetMenuItemEnabled("Statistics menu", true);
  }
  simulation_menu.SetMenuItemEnabled("Statistics menu", true);
}

void App::finishSimulation() {
  simulator->SetCurrentTime(simulator->GetStopSimulationTime());
  processEventsIfTime();
}

void App::changeDetailLevel() {
  simulator->SetHighDetailLevel(
      Menu::GetMenuChoice("Detail level, High [1], Low [0]:", 0, 1));
}

void App::searchTrainByTrainNumber() {
  int input = Menu::GetMenuChoice("Train number:", 1, 1000);
  try {
    std::string details =
        train_station_manager->GetTrainDetailsByTrainNumber(input);
    std::cout << details << "\n";
  } catch (const std::exception &e) {
    std::cout << e.what() << "\n";
  }
}

void App::searchTrainByVehicleId() {
  int input = Menu::GetMenuChoice("Train number:", 1, 1000);
  try {
    std::string details =
        train_station_manager->GetTrainDetailsByVehicleId(input);
    std::cout << details << "\n";
  } catch (const std::exception &e) {
    std::cout << e.what() << "\n";
  }
}

void App::changeTrainDetailLevel() {
  int input = Menu::GetMenuChoice("Detail level, High [1], Low [0]:", 0, 1);
  train_station_manager->SetHighLogLevelTrain(input);
}

void App::listAllStationNames() {
  std::cout << train_station_manager->ListAllStationNames();
}

void App::searchStationByName() {
  std::string name = getStringInput("Enter station name:");
  try {
    std::shared_ptr<Station> station_out =
        train_station_manager->GetStationByName(name);
    std::cout << train_station_manager->GetStationDetails(
        name, train_station_manager->IsHighLogLevelStation());
  } catch (const std::exception &e) {
    std::cout << e.what() << "\n";
  }
}

void App::changeStationDetailLevel() {
  int input = Menu::GetMenuChoice("Detail level, High [1], Low [0]:", 0, 1);
  train_station_manager->SetHighLogLevelStation(input);
}

void App::searchVehicleById() {
  int vehicle_id = Menu::GetMenuChoice("Enter vehicle id:", 1, 1000);
  try {
    std::string location;
    std::shared_ptr<Vehicle> vehicle_out =
        train_station_manager->FindVehicle(vehicle_id, location);
    if (train_station_manager->IsHighLogLevelVehicle()) {
      std::cout << vehicle_out->GetDetails() << " Location:" << location
                << "\n";
    } else {
      std::cout << vehicle_out->GetDetailsLow() << "\n";
    }
  } catch (const std::exception &e) {
    std::cout << e.what() << "\n";
  }
}

void App::changeVehicleDetailLevel() {
  int input = Menu::GetMenuChoice("Detail level, High [1], Low [0]:", 0, 1);
  train_station_manager->SetHighLogLevelVehicle(input);
}

void App::showAllStatistics() {
  std::cout << "Number of vehicles at start of simulation:\n"
            << train_station_manager->GetVehicleDistributionStart() << "\n"
            << "Total delay time: "
            << TrainTime::SecondsToPretty(
                   static_cast<int>(simulator->GetTotalDelay()))
            << "\n"
            << "Total departure delay time: "
            << TrainTime::SecondsToPretty(
                   static_cast<int>(simulator->GetTotalDepartureDelay()))
            << "\n\n"
            << "List of trains that never left station:\n"
            << train_station_manager->GetTrainsStuckAtStation() << "\n"
            << "List of delayed trains:\n"
            << train_station_manager->GetDelayedTrains() << "\n";
}

void App::showVehicleDistributionStart() {
  std::cout << "Number of vehicles at start of simulation:\n"
            << train_station_manager->GetVehicleDistributionStart() << "\n";
}

void App::showTotalDelay() {
  std::cout << "Total delay time: "
            << TrainTime::SecondsToPretty(
                   static_cast<int>(simulator->GetTotalDelay()))
            << "\n"
            << "Total departure delay time: "
            << TrainTime::SecondsToPretty(
                   static_cast<int>(simulator->GetTotalDepartureDelay()))
            << "\n";
}

void App::showTrainsStuckAtStation() {
  std::cout << "List of trains that never left station:\n"
            << train_station_manager->GetTrainsStuckAtStation() << "\n";
}

void App::showDelayedTrains() {
  std::cout << "List of delayed trains:\n"
            << train_station_manager->GetDelayedTrains() << "\n";
}

void App::showTrainsInTime() {
  std::cout << "List of trains that arrived in time:\n"
            << train_station_manager->GetTrainsThatArrivedInTime() << "\n";
}

void App::showTrainLifeCycle() {
  std::string output;
  int input = Menu::GetMenuChoice("Train number:", 1, 1000);
  if (train_station_manager->GetTrainLifeCycleByTrainNumber(
          input, output)) {
    std::cout << output;
  } else {
    std::cout << "There is no train with this number.\n";
  }
}

void App::showTrainLifeCycleByVehicleId() {
  std::string output;
  int input = Menu::GetMenuChoice("Vehicle id:", 1, 1000);
  if (train_station_manager->GetTrainLifeCycleByVehicleId(
          input, output)) {
    std::cout << output;
  } else {
    std::cout << "There is no train with this number.\n";
  }
}

void App::changeStatsDetailLevel() {
  int input = Menu::GetMenuChoice("Detail level, High [1], Low [0]:", 0, 1);
  train_station_manager->SetHighLogLevelStats(input);
}

void App::processEventsIfTime() {
  if (!simulator->RunEventsUntilTime()) {
    setSimulationDone(true);
    simulation_menu.SetMenuItemEnabled("Statistics menu", true);
    simulation_menu.SetMenuItemEnabled("Change start time", false);
    simulation_menu.SetMenuItemEnabled("Change stop time", false);
    simulation_menu.SetMenuItemEnabled("Change interval", false);
    simulation_menu.SetMenuItemEnabled("Next interval", false);
    simulation_menu.SetMenuItemEnabled("Next event", false);
    simulation_menu.SetMenuItemEnabled("Finish simulation", false);
    simulation_menu.SetMenuItemEnabled("Change detail level", false);
  }
  std::cout << TrainTime::Time_tToString(simulator->GetCurrentTime())
            << " # Current time\n";
}

bool App::keyPressNotEnter() {
  std::cout << "Press Enter to continue or Q/q for Menu\n";
  std::string choice;
  std::getline(std::cin, choice);
  return !choice.empty();
}

std::string App::getStringInput(const std::string &prompt) {
  std::cout << prompt;
  std::string line;
  std::getline(std::cin, line);
  return line;
}

time_t App::inputTime() {
  std::tm time_struct{};
  time_t time1;

  do {
    std::cout << "Input new time, hh:mm\n:";
    auto clear = []() {
      std::cin.clear();
      std::cin.ignore(std::cin.rdbuf()->in_avail());
    };
    std::cin >> std::get_time(&time_struct, "%H:%M");

    time_t today = time(nullptr);
    std::tm todayStruct{};
    gmtime_s(&todayStruct, &today);
    todayStruct.tm_hour = time_struct.tm_hour;
    todayStruct.tm_min = time_struct.tm_min;
    todayStruct.tm_sec = 0;

    time1 = mktime(&todayStruct);
    if (!std::cin.fail() && time1 >= simulator->GetCurrentTime() &&
        time1 <= simulator->GetStopTime()) {
      clear();
      return time1;
    } else {
      clear();
    }
  } while (true);
}

int App::inputInterval() {
  int time;
  auto clear = []() {
    std::cin.clear();
    std::cin.ignore(std::cin.rdbuf()->in_avail());
  };
  do {
    std::cout << "Input new interval in minutes\n:";
    std::cin >> time;
    if (!std::cin.fail() && time >= 10 &&
        time <= static_cast<int>(simulator->GetStopTime()) -
                    static_cast<int>(simulator->GetCurrentTime())) {
      clear();
      return time;
    } else {
      clear();
    }
  } while (true);
}

void App::removeOldLog() {
  std::ifstream file("Trainsim.log");
  if (file.is_open()) {
    file.close();
    std::remove("Trainsim.log");
  }
}
