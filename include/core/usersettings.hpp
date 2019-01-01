#pragma once
#ifndef USER_SETTINGS_HPP_
#define USER_SETTINGS_HPP_

#include <map>
#include <string>

// https://stackoverflow.com/questions/12116549/settings-array-in-c

/*
settings.insert("Fullscreen","true");
settings.insert("Width","1680");
settings.insert("Height","1050");
settings.insert("Title","My Application");
*/

class UserSettings {
 private:
  std::map<std::string, std::string> settings;

 public:
  UserSettings(std::string fileName);
  ~UserSettings();
};

#endif  // USER_SETTINGS_HPP_