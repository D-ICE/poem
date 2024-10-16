//
// Created by frongere on 12/04/23.
//

#include <memory>

#include "poem/poem.h"
#include <MathUtils/VectorGeneration.h>

using namespace poem;

int main() {

  // Create samples for the dimensions
  // FIXME: arange ne fonctionne vraiment pas comme voulu...
  auto STW_vector = mathutils::arange<double>(0, 20, 1);
  auto TWS_vector = mathutils::arange<double>(0, 60, 5);
  auto TWA_vector = mathutils::arange<double>(0, 180, 15);
  auto WA_vector = mathutils::arange<double>(0, 180, 15);
  auto Hs_vector = mathutils::arange<double>(0, 8, 1);

  /**
   * Create dimensions IDs
   * Here we define 3 dimensions based on their name, unit, description and bound values
   * Dimension are the unique identifiers of a dimension
   */
  auto STW = std::make_shared<Dimension>("STW_kt", "kt", "Speed Through Water", STW_vector);
  auto TWS = std::make_shared<Dimension>("TWS_kt", "kt", "True Wind Speed", TWS_vector);
  auto TWA = std::make_shared<Dimension>("TWA_deg", "deg", "True Wind Angle", TWA_vector);
  auto WA = std::make_shared<Dimension>("WA_deg", "deg", "Waves Angle", WA_vector);
  auto Hs = std::make_shared<Dimension>("Hs_m", "m", "Waves Significant Height", Hs_vector);

//  /**
//   * Create the dimension ID set
//   * The dimensionID set is used to define a N dimensional coordinate system for polars
//   * A shared_ptr is used as several polars are likely to share this coordinate system
//   * Dimensions are ordered
//   */
//  DimensionSet<5>::DimensionArray dim_ID_environment{STW, TWS, TWA, WA, Hs};
//  auto dimension_ID_environment = std::make_shared<DimensionSet<5>>(dim_ID_environment);
//
//  /**
//   * Create a dimension point set
//   * A dimension point set defines a set of coordinate points following a dimension id set that are to be used as polar
//   * coordinate sampling.
//   * A dimension point set is first populated with vector of samples data for each of the dimensions. Once done, it must
//   * be built to generate a cartesian product of every dimension and generate internally a vector of dimension points
//   */
//
//  DimensionPointGrid<5> dimension_point_grid(dimension_ID_environment);
//
//  dimension_point_grid.set_dimension_values("STW_kt", STW_vector);
//  dimension_point_grid.set_dimension_values("TWS_kt", TWS_vector);
//  dimension_point_grid.set_dimension_values("TWA_deg", TWA_vector);
//  dimension_point_grid.set_dimension_values("WA_deg", WA_vector);
//  dimension_point_grid.set_dimension_values("Hs_m", Hs_vector);
////  dimension_point_environment->build();
//
//  /**
//   * Creating attributes
//   */
//
//  Attributes attributes;
//  attributes.add_attribute("polar_type", "ND");
//
//  /**
//   * Create a polar set
//   * A polar set allows to group several polars into one unique structure. This is really what is further used
//   * to write or read on disk.
//   * The polar set has attributes that are global attributes into NetCDF files
//   *
//   * Note that a polar is composed of a pointer to a dimension ID set and a vector of polar points.
//   *
//   * A polar point is composed of a pointer to a dimension point and a value. This is really a point in the polar.
//   *
//   * A dimension point MUST be generated from a dimension point set
//   *
//   * A polar is always written following the last schema specification
//   */
//
//  auto polar_set = std::make_shared<PolarSet>(attributes, LastSchema::getInstance());
//
//  /**
//   * Create new polars using New method of polar set
//   */
//  auto brake_power = polar_set->New<double, 5>("BrakePower", "kW", "Total Brake Power of the vessel",
//                                              type::DOUBLE, dimension_point_environment);
//
//
//  double val_double = 0.;
//  for (const auto &dimension_point: *dimension_point_environment) {
//    PolarPoint<double, 5> polar_point(dimension_point, val_double);
//    brake_power->set_point(&polar_point);
//
//    val_double += 1.1;
//  }
//
//
//  std::string nc_file("test_polar.nc");
//  polar_set->to_netcdf(nc_file);

  return 0;
}