//
// Created by frongere on 03/04/23.
//

#include "PolarVersions.h"

#include <iostream>

#include "poem/exceptions.h"


namespace poem {

  PolarVersions::PolarVersions(const fs::path &scheme_folder) : m_scheme_folder(scheme_folder) {

    if (!fs::exists(scheme_folder)) {
      throw std::runtime_error("Path does not exist");
    }

    int last_version = 1;
    while (true) {
      // json scheme files are named <version>.json where version is an integer
      if (!fs::exists(version_to_file(last_version))) {
        break;
      }
      last_version++;
    }
    m_head_version = last_version - 1;

    for (int version = 1; version <= m_head_version; version++) {
      m_versions.emplace_back(version_to_file(version), version);
    }

    STOP

  }

  fs::path PolarVersions::version_to_file(int version) {
    return m_scheme_folder / (std::to_string(version) + ".json");
  }
}
