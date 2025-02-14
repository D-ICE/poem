//
// Created by frongere on 17/01/25.
//

#ifndef POEM_SPEC_V0_H
#define POEM_SPEC_V0_H

#include <string>
#include <netcdf>
#include <unordered_map>
#include "poem/enums.h"

namespace poem {

  namespace v0 {

    std::vector<std::string> mandatory_dimensions(POLAR_MODE polar_mode);

    std::unordered_map<std::string, std::vector<std::string>> mandatory_polar_tables(POLAR_MODE polar_mode);

  }  // v0

  inline bool check_v0(const std::string &filename) {
    bool compliant = true;

    // TODO

    return compliant;
  }

}  // poem

#endif //POEM_SPEC_V0_H
