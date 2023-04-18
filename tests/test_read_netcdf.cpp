//
// Created by frongere on 13/04/23.
//

#include "poem/poem.h"

using namespace poem;

int main() {

  std::string nc_file("test_polar.nc");

  PolarReader reader;

  auto polar_set = reader.Read(nc_file);

  polar_set->to_netcdf("test_polar_rewrite.nc");


  return 0;
}