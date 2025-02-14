//
// Created by frongere on 17/01/25.
//

#ifndef POEM_SPEC_V1_H
#define POEM_SPEC_V1_H

#include <string>
#include <netcdf>
#include <unordered_map>
#include "poem/enums.h"

namespace poem {

  namespace v1 {

    std::vector<std::string> mandatory_dimensions(POLAR_MODE polar_mode);

    std::unordered_map<std::string, std::vector<std::string>> mandatory_polar_tables(POLAR_MODE polar_mode);

    bool check_R1(const netCDF::NcGroup &group);

    bool check_R2(const netCDF::NcGroup &group);

    bool check_R3(const netCDF::NcGroup &group);

    bool check_R4(const netCDF::NcGroup &group);

    bool check_R5(const netCDF::NcGroup &group);

    bool check_R6(const netCDF::NcGroup &group);

    bool check_R7(const netCDF::NcGroup &group);

  }  // poem::v1

  bool check_v1(const std::string &filename);

}  // poem

#endif //POEM_SPEC_V1_H
