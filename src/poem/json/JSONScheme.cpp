//
// Created by frongere on 03/04/23.
//

#include "JSONScheme.h"

#include <iostream>
#include <vector>

namespace poem {

  JSONScheme::JSONScheme(const fs::path &folder) : m_folder(folder) {

    if (!fs::exists(folder)) {
      throw std::runtime_error("Path does not exist");
    }

    int last_version = 1;
    while (true) {
      if (!fs::exists(version_to_file(last_version))) {
        break;
      }
      last_version++;
    }
    m_head_version = last_version - 1;

    std::cout << "last version " << m_head_version << std::endl;



  }

  fs::path JSONScheme::version_to_file(int version) {
    return m_folder / (std::to_string(version) + ".json");
  }
}
