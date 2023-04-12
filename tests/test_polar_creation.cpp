//
// Created by frongere on 12/04/23.
//

#include <memory>

#include "poem/poem.h"

using namespace poem;

int main() {

  // Create dimensions IDs
  auto STW = std::make_shared<DimensionID>("STW_kt", "kt", "Speed Through Water", 0, 20);
  auto TWS = std::make_shared<DimensionID>("TWS_kt", "kt", "True Wind Speed", 0, 60);
  auto TWA = std::make_shared<DimensionID>("TWA_deg", "deg", "True Wind Angle", 0, 180);

  // Create the dimension ID array
//  auto dim_ID_array = std::make_shared<DimensionIDArray<3>>({STW, TWS, TWA});
  DimensionIDArray<3>::Array array{STW, TWS, TWA};
  auto dim_ID_array = std::make_shared<DimensionIDArray<3>>(array);

  // Create a polar set
  PolarSet polar_set;

  // Create a new polar
  auto leeway = polar_set.New<double, 3>("LEEWAY", "deg", "Leeway", type::DOUBLE, dim_ID_array);
  auto brake_power = polar_set.New<double, 3>("BrakePower", "kW", "Brake Power", type::DOUBLE, dim_ID_array);

  // Create a new dimension point
  auto dim_point = dim_ID_array->get_point();
  dim_point->set({1, 2, 3});

  // Create a polar point
  PolarPoint<double, 3> polar_point(dim_point, 5);
  leeway->push_bask({dim_point, 5});






  return 0;
}