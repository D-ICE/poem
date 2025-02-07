//
// Created by frongere on 17/01/25.
//

#ifndef POEM_CHECK_V1_H
#define POEM_CHECK_V1_H

#include <string>
#include <netcdf>

namespace poem {

  namespace v1 {

    bool check_R1(const netCDF::NcGroup &group);

    bool check_R2(const netCDF::NcGroup &group);

    bool check_R3(const netCDF::NcGroup &group);

    bool check_R4(const netCDF::NcGroup &group);

    bool check_R5(const netCDF::NcGroup &group);

    bool check_R6(const netCDF::NcGroup &group);

    bool check_R7(const netCDF::NcGroup &group);

//    bool check_R8(const netCDF::NcGroup &group);
//
//    bool check_R9(const netCDF::NcGroup &group);
//
//    bool check_R10(const netCDF::NcGroup &group);

  }  // poem::v1

  bool check_v1(const std::string &filename);

}  // poem

#endif //POEM_CHECK_V1_H
