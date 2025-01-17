//
// Created by frongere on 17/01/25.
//

#ifndef POEM_POLAR_H
#define POEM_POLAR_H

#include <string>
#include <memory>

#include "DimensionGrid.h"
#include "PolarTable.h"

namespace poem {

  /**
   * Control type of a Polar
   */
  enum CONTROL_TYPE {
    /// Velocity control
    VELOCITY_CONTROL,
    /// Power control
    POWER_CONTROL,
    /// No control
    NO_CONTROL
  };

  /**
   * The different types of available polar modes in poem
   */
  enum POLAR_MODE {
    /// Motor Power Prediction Program (motor propulsion only, VELOCITY_CONTROL)
    MPPP,
    /// Hybrid Power Prediction Program (motor and wind propulsion, VELOCITY_CONTROL)
    HPPP,
    /// Motor Velocity Prediction Program (motor propulsion only, POWER_CONTROL)
    MVPP,
    /// Hybrid Velocity Prediction Program (motor and wind propulsion, POWER_CONTROL)
    HVPP,
    /// Velocity Prediction Program (wind propulsion only, NO_CONTROL)
    VPP
  };

  /**
   * Get the control type from POLAR_MODE
   */
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

  /**
   * Converts a string representation into its corresponding POLAR_MODE
   */
  inline POLAR_MODE string_to_polar_mode(const std::string &polar_mode_str) {
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

  /**
   * Tells if a string represents a valid polar mode
   */
  bool is_polar_mode(const std::string& polar_mode_str) {
    try {
      string_to_polar_mode(polar_mode_str);
    } catch (const PoemException& e) {
      return false;
    }
    return true;
  }

  /**
   * Converts a POLAR_MODE into its corresponding string representation
   */
  inline std::string polar_mode_to_string(const POLAR_MODE &polar_mode_) {
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


  // ===================================================================================================================
  // ===================================================================================================================


  /**
   * A Polar stacks the PolarTable for one POLAR_MODE
   *
   * Every PolarTable in a Polar share the same DimensionGrid
   */
  class Polar {
   public:
    using PolarTableMap = std::unordered_map<std::string, std::shared_ptr<PolarTableBase>>;
    using PolarTableMapIter = PolarTableMap::iterator;

   public:
    Polar(const std::string &name,
          POLAR_MODE mode,
          std::shared_ptr<DimensionGrid> dimension_grid) :
        m_name(name), m_mode(mode), m_dimension_grid(dimension_grid) {

    }

    const std::string &name() const { return m_name; }

    const POLAR_MODE &mode() const { return m_mode; }

    std::shared_ptr<DimensionGrid> dimension_grid() const { return m_dimension_grid; }
    std::shared_ptr<DimensionGrid>& dimension_grid() { return m_dimension_grid; }

    template<typename T>
    std::shared_ptr<PolarTable<T>> new_polar_table(const std::string &name,
                                                   const std::string &unit,
                                                   const std::string &description,
                                                   const POEM_DATATYPE type) {

      auto polar_table = make_polar_table<T>(name, unit, description, type, m_dimension_grid);
      m_polar_tables.insert({name, polar_table});
      return polar_table;
    }

    void add_polar_table(std::shared_ptr<PolarTableBase> polar_table) {
      if (m_dimension_grid != polar_table->dimension_grid()) {
        spdlog::critical("While adding PolarTable {} to Polar {}, DimensionGrid mismatch",
                         polar_table->name(), m_name);
        CRITICAL_ERROR_POEM
      }
      m_polar_tables.insert({polar_table->name(), polar_table});
    }

    std::shared_ptr<PolarTableBase> polar(const std::string &name) const {
      return m_polar_tables.at(name);
    }

    bool operator==(const Polar& other) const {
      bool equal = true;
      // TODO: voir comment on fait pour les noms
//      bool equal = m_name == other.m_name;
      equal &= m_mode == other.m_mode;
      equal &= *m_dimension_grid == *other.m_dimension_grid;
      equal &= m_polar_tables.size() == other.m_polar_tables.size();
      for (const auto polar_table : m_polar_tables) {
        equal &= *polar_table.second == *other.m_polar_tables.at(polar_table.first);
      }
      return equal;
    }

    bool operator!=(const Polar& other) const {
      return !(other == *this);
    }

    PolarTableMapIter begin() {
      return m_polar_tables.begin();
    }

    PolarTableMapIter end() {
      return m_polar_tables.end();
    }

   private:
    std::string m_name;
    POLAR_MODE m_mode;
    std::shared_ptr<DimensionGrid> m_dimension_grid;
    PolarTableMap m_polar_tables;

  };

  std::shared_ptr<Polar> make_polar(const std::string &name,
                                    POLAR_MODE mode,
                                    std::shared_ptr<DimensionGrid> dimension_grid) {
    return std::make_shared<Polar>(name, mode, dimension_grid);
  }

}  // poem

#endif //POEM_POLAR_H
