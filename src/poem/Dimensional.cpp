//
// Created by frongere on 21/01/25.
//

#include "Dimensional.h"

namespace poem {


  Dimensional::Dimensional(const std::string &unit) :
      m_unit(unit) {

    check_unit();

    #ifndef ALLOW_DIRTY
    if (git::is_dirty()) {
        LogCriticalError("Using POEM with uncommitted code modifications is forbidden. Current POEM version: {}",
                         git::version_full());
        CRITICAL_ERROR_POEM
      }
    #endif
  }

  const std::string &Dimensional::unit() const { return m_unit; }

  void Dimensional::change_unit(const std::string &new_unit) {
    m_unit = new_unit;
    check_unit();
  }

  bool Dimensional::operator==(const Dimensional &other) const {
    return m_unit == other.m_unit;
  }

  bool Dimensional::operator!=(const Dimensional &other) const {
    return !(other == *this);
  }

  void Dimensional::check_unit() {
    if (!dunits::UnitsChecker::getInstance().is_valid_unit(m_unit, true)) {
      LogCriticalError("Unit \"{}\" is not a valid unit as per dunits library.", m_unit);
      CRITICAL_ERROR_POEM
    }
  }

}  // poem