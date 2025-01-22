//
// Created by frongere on 21/01/25.
//

#include "enums.h"
#include "exceptions.h"

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

  std::string polar_node_type_to_string(POLAR_NODE_TYPE polar_node_type) {
    std::string polar_node_type_str;
    switch (polar_node_type) {
      case POLAR_NODE:
        polar_node_type_str = "polar_node";
        break;
      case POLAR_SET:
        polar_node_type_str = "polar_set";
        break;
      case POLAR:
        polar_node_type_str = "polar";
        break;
      case POLAR_TABLE:
        polar_node_type_str = "polar_table";
        break;
    }
    return polar_node_type_str;
  }

  POLAR_NODE_TYPE string_to_polar_node_type(const std::string &polar_node_type_str) {
    NIY_POEM
  }

}  // poem