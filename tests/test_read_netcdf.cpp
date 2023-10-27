//
// Created by frongere on 13/04/23.
//

#include <string>

#include <gtest/gtest.h>
#include "poem/poem.h"

using namespace poem;

TEST(poem_, reader) {

  PolarSetReader reader;

  reader.Read("polar_dev.nc");

  auto polar_set = reader.polar_set();
  auto attributes = reader.attributes();

//  polar_set->to_netcdf("polar_dev_rewrite.nc");

}
