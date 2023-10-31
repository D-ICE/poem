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

  std::array<double, 5> dimension_point{10.1, 10.1, 90.1, 0, 0};

  auto polar_double = polar_set->polar<double, 5>("TotalBrakePower");
//  poem::DimensionPoint<5>()

  auto res = polar_double->nearest(dimension_point, true);
  std::cout << res << std::endl;

  auto interp = polar_double->interp(dimension_point, true);
  std::cout << interp << std::endl;

//  polar_set->to_netcdf("polar_dev_rewrite.nc");

  auto polar_int = polar_set->polar<int, 5>("HasConstraintViolation");
  std::cout << polar_int->nearest(dimension_point, true) << std::endl;
//  std::cout << polar_nb_iter->interp(dimension_point, true) << std::endl; // Not possible


}
