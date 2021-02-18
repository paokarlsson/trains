/**
 * \author [Ola Karlsson](mailto:olka0600@student.miun.se)
 * \copyright Copyright 2020 Ola Karlsson. All rights reserved.
 */

#ifndef PROJECT_INCLUDE_MENU_H_
#define PROJECT_INCLUDE_MENU_H_

#include <functional>
#include <iosfwd>
#include <list>
#include <string>
#include <utility>

/** \brief Class for holding one menu item.
 * Each menu item is loaded into a specific menu.
 * Each menu item has a name, if menu is enabled or not and an action to take
 * if this item is choosen.
 * The action is an alternative to a funcion pointer. This was more straight
 * forward to implement because the functions was member functions.
 */
class MenuItem {
  std::string name_;
  bool enabled_;
  std::function<void()> action_;

 public:
  MenuItem(const std::string &name, bool enabled, std::function<void()> action)
      : name_(name), enabled_(enabled), action_(action) {}
  ~MenuItem() {}
  bool IsEnabled() { return enabled_; }
  void SetEnabled(bool enabled) { enabled_ = enabled; }
  std::string GetName() { return name_; }
  void Run();
};

/** \brief This is a class for setting up a menu.
 * The menu consists of a number of menu items stored in a list.
 * The class has two important funcions, PrintMenu and DoChoice.
 * The PrintMenu-function shows all enabled menu items for the user.
 * The DoChoice-function takes user input and executes the action function
 * stored in the menu item.
 */
class Menu {
  bool is_main_;
  std::string header_;
  std::list<MenuItem> menu_item_;
  std::list<std::pair<int, MenuItem>> choice_map_;

 public:
  Menu(const std::string &header, bool is_main)
      : header_(header), is_main_(is_main) {}
  ~Menu() {}
  void PrintMenu();
  bool DoChoice();
  static int GetMenuChoice(const std::string &prompt, int low, int high);
  bool IsMain() { return is_main_; }
  void SetIsMain(bool is_main) { is_main_ = is_main; }
  void SetMenuItemEnabled(const std::string &name, bool isEnabled);
  void AddMenuItem(MenuItem menu_item);
};

#endif  // PROJECT_INCLUDE_MENU_H_
