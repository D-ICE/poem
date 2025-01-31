//
// Created by frongere on 21/01/25.
//

#include "Named.h"

namespace poem {


  Named::Named(const std::string &unit, const std::string &description) :
      m_unit(unit),
      m_description(description) {

    check_unit();

    #ifndef ALLOW_DIRTY
    if (git::is_dirty()) {
        LogCriticalError("Using POEM with uncommitted code modifications is forbidden. Current POEM version: {}",
                         git::version_full());
        CRITICAL_ERROR_POEM
      }
    #endif
  }

  const std::string &Named::unit() const { return m_unit; }

  void Named::change_unit(const std::string &new_unit) { m_unit = new_unit; }

  const std::string &Named::description() const { return m_description; }

  void Named::change_description(const std::string &new_description) { m_description = new_description; }

  bool Named::operator==(const Named &other) const {
    return m_unit == other.m_unit
           && m_description == other.m_description;
  }

  bool Named::operator!=(const Named &other) const {
    return !(other == *this);
  }

  void Named::check_unit() {
    if (!dunits::UnitsChecker::getInstance().is_valid_unit(m_unit, true)) {
      LogCriticalError("Unit \"{}\" is not a valid unit as per dunits library.", m_unit);
      CRITICAL_ERROR_POEM
    }
  }

}  // poem