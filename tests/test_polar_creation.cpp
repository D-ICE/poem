//
// Created by frongere on 12/04/23.
//

#include <memory>

#include "poem/poem.h"
#include <MathUtils/VectorGeneration.h>

using namespace poem;

int main() {

  /**
   * Create dimensions IDs
   * Here we define 3 dimensions based on their name, unit, description and bound values
   * DimensionID are the unique identifiers of a dimension
   */
  auto STW = std::make_shared<DimensionID>("STW_kt", "kt", "Speed Through Water", 0, 20);
  auto TWS = std::make_shared<DimensionID>("TWS_kt", "kt", "True Wind Speed", 0, 60);
  auto TWA = std::make_shared<DimensionID>("TWA_deg", "deg", "True Wind Angle", 0, 180);

  /**
   * Create the dimension ID set
   * The dimensionID set is used to define a N dimensional coordinate system for polars
   * A shared_ptr is used as several polars are likely to share this coordinate system
   * Dimensions are ordered
   */
  DimensionIDSet<3>::Array array{STW, TWS, TWA};
  auto dimension_ID_set = std::make_shared<DimensionIDSet<3>>(array);

  /**
   * Create a dimension point set
   * A dimension point set defines a set of coordinate points following a dimension id set that are to be used as polar
   * coordinate sampling.
   * A dimension point set is first populated with vector of samples data for each of the dimensions. Once done, it must
   * be built to generate a cartesian product of every dimension and generate internally a vector of dimension points
   */
  auto dimension_point_set = std::make_shared<DimensionPointSet<3>>(dimension_ID_set);

  // Create samples for the dimensions
  // FIXME: arange ne fonctionne vraiment pas comme voulu...
  dimension_point_set->set_dimension_vector("STW_kt", mathutils::arange<double>(0, 20, 1));
  dimension_point_set->set_dimension_vector("TWS_kt", mathutils::arange<double>(0, 60, 5));
  dimension_point_set->set_dimension_vector("TWA_deg", mathutils::arange<double>(0, 180, 15));

  dimension_point_set->build();


  /**
   * Create a polar set
   * A polar set allows to group several polars into one unique structure. This is really what is further used
   * to write or read on disk.
   * The polar set has attributes that are global attributes into NetCDF files
   *
   * Note that a polar is composed of a pointer to a dimension ID set and a vector of polar points.
   *
   * A polar point is composed of a pointer to a dimension point and a value. This is really a point in the polar.
   *
   * A dimension point MUST be generated from a dimension point set
   */
  PolarSet polar_set;

  // TODO: define and add attributes...


  /**
   * Create new polars using New method of polar set
   */
  auto leeway = polar_set.New<double, 3>("LEEWAY", "deg", "Leeway", type::DOUBLE, dimension_point_set);


  for (const auto& dimension_point : *dimension_point_set) {
    PolarPoint<double, 3> polar_point(dimension_point, 10.);

    leeway->set_point(&polar_point);
  }

//  std::cout << leeway->is_filled() << std::endl;


  return 0;
}