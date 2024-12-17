//
// Created by frongere on 17/12/24.
//

#include <gtest/gtest.h>
#include <poem/poem.h>

using namespace poem;

std::shared_ptr<InterpolablePolarTable<3>> create_polar_table() {

  // Create dimensions
  auto STW_dim = std::make_shared<poem::Dimension>("STW_kt", "kt", "Speed Through Water");
  auto TWS_dim = std::make_shared<poem::Dimension>("TWS_kt", "kt", "True Wind Speed");
  auto TWA_dim = std::make_shared<poem::Dimension>("TWA_deg", "deg", "True Wind Angle");

  // Create DimensionSet
  std::array<std::shared_ptr<poem::Dimension>, 3> array{STW_dim, TWS_dim, TWA_dim};
  auto dimension_set = std::make_shared<poem::DimensionSet<3>>(array);

  // Create sampled for dimension
  std::vector<double> STW_vector = {1, 2, 3};
  std::vector<double> TWS_vector = {1, 2, 3};
  std::vector<double> TWA_vector = {1, 2, 3};

  // DimensionGrid to generate DimensionPointSet
  auto dimension_grid = poem::DimensionGrid(dimension_set);
  dimension_grid.set_values("STW_kt", STW_vector);
  dimension_grid.set_values("TWS_kt", TWS_vector);
  dimension_grid.set_values("TWA_deg", TWA_vector);

  // Get the dimension point set
  auto dimension_point_set = std::make_shared<poem::DimensionPointSet<3>>(dimension_grid);

  // Visu of points
//  for (const auto &dimension_point: *dimension_point_set) {
//    std::cout << dimension_point << std::endl;
//  }

  auto polar_table = std::make_shared<InterpolablePolarTable<3>>(
      "VAR", "-", "Variable",
      type::POEM_TYPES::DOUBLE,
      poem::POLAR_TYPE::HVPP,
      dimension_point_set
  );

  // Filling the polar table
  size_t idx = 0;
  for (const auto &dimension_point : *dimension_point_set) {
    double val = 1.;
    for (const auto coord : dimension_point) {
      val *= coord;
    }

    polar_table->set_value(idx, val);
    idx++;
  }

  return polar_table;
}


TEST(poem_polar_slicing, poem_polar_slicing) {

  // Get a polar table
  auto polar_table = create_polar_table();

  std::unordered_map<std::string, double> slice1{{"STW_kt", 1.2},
                                                 {"TWS_kt", 2.05}};

  auto sliced1 = polar_table->slice(slice1);

  for (const auto& dimension_point : *sliced1->dimension_point_set()) {
    double val = 1.;
    for (const auto &coord : dimension_point) {
      val *= coord;
    }
    ASSERT_EQ(polar_table->interp(dimension_point.array(), false), val);
  }

}