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
  auto d1 = std::make_shared<DimensionID>("d1", "-", "d1", 0, 20);
  auto d2 = std::make_shared<DimensionID>("d2", "-", "d2", 0, 60);
  auto d3 = std::make_shared<DimensionID>("d3", "-", "d3", 0, 180);
  auto d4 = std::make_shared<DimensionID>("d4", "-", "d4", 0, 50);

  /**
   * Create the dimension ID set
   * The dimensionID set is used to define a N dimensional coordinate system for polars
   * A shared_ptr is used as several polars are likely to share this coordinate system
   * Dimensions are ordered
   */
  DimensionIDSet<3>::IDSet dim_ID_set_double{d1, d2, d3};
  auto dimension_ID_set_double = std::make_shared<DimensionIDSet<3>>(dim_ID_set_double);

  DimensionIDSet<2>::IDSet dim_ID_set_int{d1, d4};
  auto dimension_ID_set_int = std::make_shared<DimensionIDSet<2>>(dim_ID_set_int);

  /**
   * Create a dimension point set
   * A dimension point set defines a set of coordinate points following a dimension id set that are to be used as polar
   * coordinate sampling.
   * A dimension point set is first populated with vector of samples data for each of the dimensions. Once done, it must
   * be built to generate a cartesian product of every dimension and generate internally a vector of dimension points
   */


  // Create samples for the dimensions
  // FIXME: arange ne fonctionne vraiment pas comme voulu...
  auto d1_vector = mathutils::arange<double>(0, 20, 1);
  auto d2_vector = mathutils::arange<double>(0, 60, 5);
  auto d3_vector = mathutils::arange<double>(0, 180, 15);
  auto d4_vector = mathutils::arange<double>(0, 50, 2);

  auto dimension_point_set_var_double = std::make_shared<DimensionPointSet<3>>(dimension_ID_set_double);
  dimension_point_set_var_double->set_dimension_vector("d1", d1_vector);
  dimension_point_set_var_double->set_dimension_vector("d2", d2_vector);
  dimension_point_set_var_double->set_dimension_vector("d3", d3_vector);
  dimension_point_set_var_double->build();

  auto dimension_point_set_var_int = std::make_shared<DimensionPointSet<2>>(dimension_ID_set_int);
  dimension_point_set_var_int->set_dimension_vector("d1", d1_vector);
  dimension_point_set_var_int->set_dimension_vector("d4", d4_vector);
  dimension_point_set_var_int->build();

  /**
   * Creating attributes
   */

  Attributes attributes;
  attributes.add_attribute("att_mandatory", "mandatory attribute");
  attributes.add_attribute("att_optional", "optional attribute");
  attributes.add_attribute("attopt_glob", "optional attribute with glob syntax");

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
  auto polar_set = std::make_shared<PolarSet>(attributes);

  /**
   * Create new polars using New method of polar set
   */
  auto var_double = polar_set->New<double, 3>("var_double", "-", "double variable with dimensions [d1, d2, d3]",
                                              type::DOUBLE, dimension_point_set_var_double);
  auto var_int = polar_set->New<int, 2>("var_int", "-", "int variable with dimensions [d1, d4]",
                                        type::INT, dimension_point_set_var_int);


  double val_double = 0.;
  for (const auto &dimension_point: *dimension_point_set_var_double) {
    PolarPoint<double, 3> polar_point(dimension_point, val_double);
    var_double->set_point(&polar_point);

    val_double += 1.1;
  }

  int val_int = 0;
  for (const auto &dimension_point: *dimension_point_set_var_int) {
    PolarPoint<int, 2> polar_point(dimension_point, val_int);
    var_int->set_point(&polar_point);

    val_int += 1;
  }

  SchemaChecker::getInstance().check(polar_set.get());

  std::string nc_file("test_polar.nc");
  polar_set->to_netcdf(nc_file);

  return 0;
}