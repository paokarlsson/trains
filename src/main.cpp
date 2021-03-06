/**
 * \author [Ola Karlsson](mailto:olka0600@student.miun.se)
 * \copyright Copyright 2020 Ola Karlsson. All rights reserved.
 */

#include <iostream>

#include "app.h"  // NOLINT
#include "memstat.hpp"

int main() {
  {
    try {
      App app("../train-data/TrainStations.txt",
              "../train-data/Trains.txt",
              "../train-data/TrainMap.txt");

      app.Run();
    } catch (const std::exception& e) {
      std::cout << e.what() << "\n";
    }
  }
}
