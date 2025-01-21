//
// Created by frongere on 21/01/25.
//

#include "OperationMode.h"

namespace poem {

  std::string clean_name(const std::string &str) {
    // FIXME: semblerait que la fonctionnalite soit aussi dans cools...
    // FIXME: removes also numeric characters

    std::string s = str;
    for (size_t i = 0; i < s.size(); i++) {
      if (s[i] == ' ') {
        s[i] = '_';
        continue;
      }
      if (s[i] == '_') continue;

      if (s[i] < 'A' || s[i] > 'Z' && s[i] < 'a' || s[i] > 'z') {
        s.erase(i, 1);
        i--;
      }
    }
    if (str != s) {
      spdlog::warn("Name \"{}\" has been changed in \"{}\"", str, s);
    }

    return s;
  }

  std::shared_ptr<PolarSet> OperationMode::create_polar_set(const std::string &name) {}

  std::shared_ptr<PolarSet> OperationMode::polar_set() const {

    if (!is_leaf()) {
      spdlog::critical("This OperationMode {} cannot handle a PolarSet (not a leaf)", full_name().string());
      CRITICAL_ERROR_POEM
    }
    if (!m_polar_set) {
      spdlog::critical("In OperationMode {}, the PolarSet has not been set yet", full_name().string());
    }

    return m_polar_set;
  }

  void OperationMode::set_polar_set(std::shared_ptr<PolarSet> polar_set) {
    if (m_polar_set) {
      spdlog::critical("In OperationMode {}, the PolarSet already exists", full_name().string());
      CRITICAL_ERROR_POEM
    }
    m_polar_set = polar_set;
  }

  bool OperationMode::has_polar_set() const {
    return m_polar_set != nullptr;
  }

  bool OperationMode::operator==(const OperationMode &other) const {
    NIY_POEM
  }

  bool OperationMode::operator!=(const OperationMode &other) const {
    return !(other == *this);
  }

}  // poem
