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
  CONTROL_TYPE control_type(POLAR_MODE polar_mode);

  /**
   * Converts a string representation into its corresponding POLAR_MODE
   */
  POLAR_MODE string_to_polar_mode(const std::string &polar_mode_str);

  /**
   * Tells if a string represents a valid polar mode
   */
  bool is_polar_mode(const std::string &polar_mode_str);

  /**
   * Converts a POLAR_MODE into its corresponding string representation
   */
  std::string polar_mode_to_string(const POLAR_MODE &polar_mode_);

  // ===================================================================================================================
  // ===================================================================================================================

  // Forward declaration
  class PolarSet;

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
        m_name(name), m_mode(mode), m_dimension_grid(dimension_grid) {}

    const std::string &name() const;

    std::string full_name() const;

    const POLAR_MODE &mode() const;

    std::shared_ptr<DimensionGrid> dimension_grid() const;

    std::shared_ptr<DimensionGrid> &dimension_grid();

    template<typename T>
    std::shared_ptr<PolarTable<T>> new_polar_table(const std::string &name,
                                                   const std::string &unit,
                                                   const std::string &description,
                                                   const POEM_DATATYPE type);

    void add_polar_table(std::shared_ptr<PolarTableBase> polar_table);

    std::shared_ptr<PolarTableBase> polar(const std::string &name) const;

    bool operator==(const Polar &other) const;

    bool operator!=(const Polar &other) const;

    PolarTableMapIter begin();

    PolarTableMapIter end();

   private:
    std::string m_name;
    POLAR_MODE m_mode;
    std::shared_ptr<DimensionGrid> m_dimension_grid;
    PolarTableMap m_polar_tables;

    std::shared_ptr<PolarSet> m_polar_set_parent;

  };

  template<typename T>
  std::shared_ptr<PolarTable<T>>
  Polar::new_polar_table(const std::string &name, const std::string &unit, const std::string &description,
                         const POEM_DATATYPE type) {

    auto polar_table = make_polar_table<T>(name, unit, description, type, m_dimension_grid);
    m_polar_tables.insert({name, polar_table});
    return polar_table;
  }

  std::shared_ptr<Polar> make_polar(const std::string &name,
                                    POLAR_MODE mode,
                                    std::shared_ptr<DimensionGrid> dimension_grid);

}  // poem

#endif //POEM_POLAR_H
