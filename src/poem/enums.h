//
// Created by frongere on 21/01/25.
//

#ifndef POEM_ENUMS_H
#define POEM_ENUMS_H

#include <string>

namespace poem {

  /**
   * PolarTable datatype
   */
  enum POEM_DATATYPE {
    /// double
    POEM_DOUBLE,
    /// int
    POEM_INT,
  };

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

  enum POLAR_NODE_TYPE {
    /// Regular PolarNode
    POLAR_NODE,
    /// PolarSet
    POLAR_SET,
    /// Polar
    POLAR,
    /// PolarTableBase
    POLAR_TABLE
  };

}  // poem

#endif //POEM_ENUMS_H
