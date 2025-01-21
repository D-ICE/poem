//
// Created by frongere on 21/01/25.
//

#include "Polar.h"

#include "PolarSet.h"

namespace poem {

  CONTROL_TYPE control_type(POLAR_MODE polar_mode) {
    CONTROL_TYPE control_type;
    switch (polar_mode) {
      case MPPP:
        control_type = VELOCITY_CONTROL;
        break;
      case HPPP:
        control_type = VELOCITY_CONTROL;
        break;
      case MVPP:
        control_type = POWER_CONTROL;
        break;
      case HVPP:
        control_type = POWER_CONTROL;
        break;
      case VPP:
        control_type = NO_CONTROL;
        break;
    }
    return control_type;
  }

  POLAR_MODE string_to_polar_mode(const std::string &polar_mode_str) {
    POLAR_MODE polar_mode;
    if (polar_mode_str == "MPPP") {
      polar_mode = MPPP;
    } else if (polar_mode_str == "HPPP") {
      polar_mode = HPPP;
    } else if (polar_mode_str == "HVPP") {
      polar_mode = HVPP;
    } else if (polar_mode_str == "MVPP") {
      polar_mode = MVPP;
    } else if (polar_mode_str == "VPP") {
      polar_mode = VPP;
//    } else if (polar_mode_str == "ND") {
//      // This is the v0 version of POEM specs...
//      polar_mode = HPPP;
//    } else if (polar_mode_str == "/") {
//      // This is the root group and might be a v0 version of POEM specs...
//      polar_mode = HPPP;
    } else {
      spdlog::critical("Polar type \"{}\" unknown", polar_mode);
      CRITICAL_ERROR_POEM
    }
    return polar_mode;
  }

  bool is_polar_mode(const std::string &polar_mode_str) {
    try {
      string_to_polar_mode(polar_mode_str);
    } catch (const PoemException &e) {
      return false;
    }
    return true;
  }

  std::string polar_mode_to_string(const POLAR_MODE &polar_mode_) {
    std::string polar_type;
    switch (polar_mode_) {
      case MPPP:
        polar_type = "MPPP";
        break;
      case HPPP:
        polar_type = "HPPP";
        break;
      case HVPP:
        polar_type = "HVPP";
        break;
      case MVPP:
        polar_type = "MVPP";
        break;
      case VPP:
        polar_type = "VPP";
        break;
    }
    return polar_type;
  }

  const std::string &Polar::name() const { return m_name; }

  std::string Polar::full_name() const {
    std::string full_name_;
    if (m_polar_set_parent) {
      full_name_ = m_polar_set_parent->full_name() + "/" + m_name;
    } else {
      full_name_ = "/" + m_name;
    }
    return full_name_;
  }

  const POLAR_MODE &Polar::mode() const { return m_mode; }

  std::shared_ptr<DimensionGrid> Polar::dimension_grid() const { return m_dimension_grid; }

  std::shared_ptr<DimensionGrid> &Polar::dimension_grid() { return m_dimension_grid; }

  void Polar::add_polar_table(std::shared_ptr<PolarTableBase> polar_table) {
    if (m_dimension_grid != polar_table->dimension_grid()) {
      spdlog::critical("While adding PolarTable {} to Polar {}, DimensionGrid mismatch",
                       polar_table->name(), m_name);
      CRITICAL_ERROR_POEM
    }
    polar_table->set_polar_parent(this);

    m_polar_tables.insert({polar_table->name(), polar_table});
  }

  std::shared_ptr<PolarTableBase> Polar::polar_table(const std::string &name) const {
    return m_polar_tables.at(name);
  }

  bool Polar::operator==(const Polar &other) const {
    bool equal = m_name == other.m_name;
    equal &= m_mode == other.m_mode;
    equal &= *m_dimension_grid == *other.m_dimension_grid;
    equal &= m_polar_tables.size() == other.m_polar_tables.size();
    for (const auto &polar_table: m_polar_tables) {
      equal &= *polar_table.second == *other.m_polar_tables.at(polar_table.first);
    }
    return equal;
  }

  bool Polar::operator!=(const Polar &other) const {
    return !(other == *this);
  }

  Polar::PolarTableMapIter Polar::begin() {
    return m_polar_tables.begin();
  }

  Polar::PolarTableMapIter Polar::end() {
    return m_polar_tables.end();
  }

  void Polar::set_polar_set_parent(PolarSet *polar_set) {
    m_polar_set_parent = polar_set;
  }

  std::shared_ptr<Polar>
  make_polar(const std::string &name, POLAR_MODE mode, std::shared_ptr<DimensionGrid> dimension_grid) {
    return std::make_shared<Polar>(name, mode, dimension_grid);
  }

}  // poem
