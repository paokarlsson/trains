/**
 * \author [Ola Karlsson](mailto:olka0600@student.miun.se)
 * \copyright Copyright 2020 Ola Karlsson. All rights reserved.
 */

#include "menu.h" //NOLINT

#include <algorithm>
#include <iostream>
#include <list>

void MenuItem::Run() {
  try {
    action_();
  } catch (const std::exception &e) {
    std::cout << "Something went wrong!\n";
  }
}

void Menu::AddMenuItem(MenuItem menu_item) { menu_item_.push_back(menu_item); }

void Menu::PrintMenu() {
  std::cout << header_ << "\n\n";
  choice_map_.clear();
  int i = 1;
  std::for_each(menu_item_.begin(), menu_item_.end(), [&](MenuItem &menu_item) {
    if (menu_item.IsEnabled()) {
      std::cout << "[" << i << "] " << menu_item.GetName() << "\n";
      choice_map_.push_back(std::make_pair(i, menu_item));
      i++;
    }
  });
  std::cout << "[0] " << (IsMain() ? "Exit" : "Back") << "\n";
}

bool Menu::DoChoice() {
  int choice = GetMenuChoice(":", 0, choice_map_.size());
  if (choice != 0) {
    auto it = std::find_if(choice_map_.begin(), choice_map_.end(),
                           [&choice](std::pair<int, MenuItem> &choice_item) {
                             if (choice_item.first == choice) {
                               choice_item.second.Run();
                               return true;
                             } else {
                               return false;
                             }
                           });
    return true;
  } else {
    return false;
  }
}

int Menu::GetMenuChoice(const std::string &prompt, int low, int high) {
  // Lambda to declare a function within a function.
  auto Clear = []() {
    std::cin.clear();
    std::cin.ignore(std::cin.rdbuf()->in_avail());
  };
  do {
    std::cout << prompt;
    int choice;
    std::cin >> choice;
    if (!std::cin.fail() && choice >= low && choice <= high) {
      Clear();
      return choice;
    } else {
      Clear();
    }
  } while (true);
}

void Menu::SetMenuItemEnabled(const std::string &name, bool is_enabled) {
  auto it = std::find_if(menu_item_.begin(), menu_item_.end(),
                         [&name, is_enabled](MenuItem &mi) {
                           if (mi.GetName() == name) {
                             mi.SetEnabled(is_enabled);
                             return true;
                           } else {
                             return false;
                           }
                         });
}
