//
// Created by frongere on 17/01/25.
//

#ifndef POEM_SPECS_H
#define POEM_SPECS_H

#include "poem/exceptions.h"

#include "spec_v0.h"
#include "spec_v1.h"

namespace poem {

  inline bool spec_check(const std::string &filename, int major_version) {

    if (!fs::exists(filename)) {
      LogCriticalError("NetCDF file not found: {}", filename);
      CRITICAL_ERROR_POEM
    }

    bool compliant;
    switch (major_version) {
      case 0:
        compliant = check_v0(filename);
        break;
      case 1:
        compliant = check_v1(filename);
        break;
      default:
        LogCriticalError("POEM specification v{} does not exist", major_version);
        CRITICAL_ERROR_POEM
    }

    return compliant;
  }

  inline std::vector<std::string> mandatory_dimensions(POLAR_MODE polar_mode, int major_version) {
    std::vector<std::string> mandatory_dimensions;
    switch (major_version) {
      case 0:
        mandatory_dimensions = v0::mandatory_dimensions(polar_mode);
        break;
      case 1:
        mandatory_dimensions = v1::mandatory_dimensions(polar_mode);
        break;
      default:
        LogCriticalError("POEM specification v{} does not exist", major_version);
        CRITICAL_ERROR_POEM
    }
    return mandatory_dimensions;
  }

  inline std::unordered_map<std::string, std::vector<std::string>>
  mandatory_polar_tables(POLAR_MODE polar_mode, int major_version) {
    std::unordered_map<std::string, std::vector<std::string>> mandatory_polar_tables;
    switch (major_version) {
      case 0:
        mandatory_polar_tables = v0::mandatory_polar_tables(polar_mode);
        break;
      case 1:
        mandatory_polar_tables = v1::mandatory_polar_tables(polar_mode);
        break;
      default:
        LogCriticalError("POEM specification v{} does not exist", major_version);
        CRITICAL_ERROR_POEM
    }
    return mandatory_polar_tables;
  }

}  // poem

#endif //POEM_SPECS_H
