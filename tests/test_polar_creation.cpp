//
// Created by frongere on 12/04/23.
//

#include <memory>

#include "poem/poem.h"
#include <MathUtils/VectorGeneration.h>

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
//  auto dim_point = dim_ID_array->get_point();
//  dim_point->set({1, 2, 3});




  // Create a DimensionSampleArray
  DimensionSampleArray<3> dimension_sample_array(dim_ID_array);

  // Create samples for the dimensions
  // FIXME: arange ne fonctionne vraiment pas comme voulu...
  dimension_sample_array.set("STW_kt", mathutils::arange<double>(0, 20, 1));
  dimension_sample_array.set("TWS_kt", mathutils::arange<double>(0, 60, 5));
  dimension_sample_array.set("TWA_deg", mathutils::arange<double>(0, 180, 15));

  auto dim_points = dimension_sample_array.get_dimension_points_vector();

  // Create a polar point
  for (const auto& dim_point : dim_points) {
    PolarPoint<double, 3> polar_point(dim_point, 5);
    leeway->push_bask({dim_point, 5});
    brake_power->push_bask({dim_point, 100});
  }









  return 0;
}