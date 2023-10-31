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

//  auto polar_names = polar_set->polar_names();
//  for (const auto &name : polar_names) {
//    auto polar = polar_set->polar(name);
//
//    std::cout << polar->name() << std::endl;
//  }

  auto polar = polar_set->polar<double, 5>("TotalBrakePower");
//  poem::DimensionPoint<5>()

  auto res = polar->nearest({10, 10, 90, 0, 0}, true);

  std::cout << res << std::endl;

//  polar_set->to_netcdf("polar_dev_rewrite.nc");

}
