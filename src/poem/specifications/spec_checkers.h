//
// Created by frongere on 17/01/25.
//

#ifndef POEM_SPEC_CHECKERS_H
#define POEM_SPEC_CHECKERS_H

#include "poem/exceptions.h"

#include "check_v0.h"
#include "check_v1.h"

namespace poem {

  bool spec_check(const std::string& filename, int major_version) {

    if (!fs::exists(filename)){
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

}  // poem

#endif //POEM_SPEC_CHECKERS_H
