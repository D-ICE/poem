//
// Created by frongere on 11/04/23.
//

#include "PolarWriter.h"

#include <iostream>
#include <netcdf>

#include "poem/polar/Polar.h"

namespace poem {

//  int PolarWriter::write(const std::string &nc_file) const {
//
//    // Return this in event of a problem
//    constexpr int nc_err = 2;
//
//    try {
//
//      // Create the file. The Replace parameter tells netCDF to overwrite
//      // this file, if it already exists.
//      netCDF::NcFile dataFile(nc_file, netCDF::NcFile::replace);
//
//      // TODO: Ecrire les attributs
//
//      for (const auto &polar: *m_polar_set) {
//        polar->to_netcdf(dataFile);
//      }
//
//    } catch (netCDF::exceptions::NcException &e) {
//      std::cerr << e.what() << std::endl;
//      return nc_err;
//    }
//
//
//    return 0;
//  }


}  // poem
