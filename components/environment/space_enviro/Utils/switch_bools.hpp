//
// Created by SoliareofAstora on 10.02.2020.
//

#ifndef SPACE_ENVIRO_UTILS_SWITCHABLEBOOLS_HPP_
#define SPACE_ENVIRO_UTILS_SWITCHABLEBOOLS_HPP_

#include <vector>
#include <map>

class SwitchBools {

  std::map<std::string, int> names;
  std::vector<bool> values;
 public:
  SwitchBools(const std::vector<std::string> &switchNames) {
    for (int i = 0; i < switchNames.size(); ++i) {
      names[switchNames[i]] = i;
    }

    values.reserve(3 * names.size());
    for (int i = 0; i < names.size(); ++i) {
      values.push_back(false);
      values.push_back(true);
      values.push_back(false);
    }
  };

  void Update() {
    for (int i = 0; i < names.size(); ++i) {
      if (values[3 * i + 2]) {
        if (values[3 * i + 1]) {
          values[3 * i].flip();
          values[3 * i + 1] = false;
        }
      } else {
        values[3 * i + 1] = true;
      }
      values[3 * i + 2] = false;
    }
  }

  void Press(const std::string name) {
    values[3 * names[name] + 2] = true;
  }

  bool operator[](const std::string name) {
    return values[3 * names[name]];
  }
};


#endif //SPACE_ENVIRO_UTILS_SWITCHABLEBOOLS_HPP_