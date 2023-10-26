//
// Created by frongere on 13/04/23.
//

#include "poem/poem.h"

using namespace poem;

int main() {

  std::string nc_file("polar_dev.nc");

  PolarReader reader;

  reader.Read(nc_file);

  auto polar_set = reader.polar_set();
  auto attributes = reader.attributes();

//  polar_set->to_netcdf("test_polar_rewrite.nc");


  return 0;
}