//
// Created by frongere on 13/04/23.
//

#include <string>
#include <filesystem>

#include <gtest/gtest.h>
#include "poem/poem.h"

using namespace poem;

namespace fs = std::filesystem;

TEST(poem_, reader) {

  PolarSetReader reader;

  std::string polar_file = fs::path(POEM_RESOURCE_DIR) / "polar_dev.nc";

  reader.Read(polar_file);

  auto polar_set = reader.polar_set();
  auto attributes = reader.attributes();

//  polar_set->to_netcdf("polar_dev_rewrite.nc");

}
