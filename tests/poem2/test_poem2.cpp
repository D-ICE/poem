//
// Created by frongere on 17/12/24.
//

#include <gtest/gtest.h>
#include <poem2/poem2.h>

using namespace poem2;



TEST(poem2, polar_table) {

  // Dimensions
  auto STW = make_dimension("STW");
  auto TWS = make_dimension("TWS");
  auto TWA = make_dimension("TWA");

  // DimensionSet
  auto dimension_set = make_dimension_set({STW, TWS, TWA});

  // DimensionGrid
  auto dimension_grid = make_dimension_grid(dimension_set);
  dimension_grid->set_values("TWA", {3, 4, 5});

  ASSERT_ANY_THROW(dimension_grid->dimension_points());

  dimension_grid->set_values("STW", {1, 4, 5});
  dimension_grid->set_values("TWS", {2, 4, 5});


  ASSERT_NO_THROW(dimension_grid->dimension_points());
//  auto dimension_points = dimension_grid->dimension_points();


  // PolarTable
  auto polar_table = make_polar_table<double>(dimension_grid);

  auto dimension_points = polar_table->dimension_points();

  // Filling the polar with values
  size_t idx = 0;
  for (const auto &dimension_point: polar_table->dimension_points()) {
    double val = 1.;
    for (const auto coord: dimension_point) {
      val *= coord;
    }

    polar_table->set_value(idx, val);
    idx++;
  }


  return;
}