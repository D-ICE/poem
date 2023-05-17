//
// Created by frongere on 26/04/23.
//

#include <gtest/gtest.h>
#include <poem/poem.h>
#include <MathUtils/VectorGeneration.h>
#include <fstream>


using namespace poem;

std::string get_json_str_from_file(const std::string &filename) {
  auto schema_dir = fs::path(TEST_SCHEMAS_DIR);
  std::ifstream ifs(schema_dir / filename);
  std::stringstream buffer;
  buffer << ifs.rdbuf();
  return buffer.str();
}


TEST(POLAR, WRITE) {

  /**
   * Create dimensions IDs
   * Here we define 3 dimensions based on their name, unit, description and bound values
   * DimensionID are the unique identifiers of a dimension
   */
  auto STW = std::make_shared<DimensionID>("STW_kt", "kt", "Speed Through Water", 0, 20);
  auto TWS = std::make_shared<DimensionID>("TWS_kt", "kt", "True Wind Speed", 0, 60);
  auto TWA = std::make_shared<DimensionID>("TWA_deg", "deg", "True Wind Angle", 0, 180);
  auto WA = std::make_shared<DimensionID>("WA_deg", "deg", "Waves Angle", 0, 180);
  auto Hs = std::make_shared<DimensionID>("Hs_m", "m", "Waves Significant Height", 0, 8);

  /**
   * Create the dimension ID set
   * The dimensionID set is used to define a N dimensional coordinate system for polars
   * A shared_ptr is used as several polars are likely to share this coordinate system
   * Dimensions are ordered
   */
  DimensionIDSet<5>::IDSet dim_ID_environment{STW, TWS, TWA, WA, Hs};
  auto dimension_ID_environment = std::make_shared<DimensionIDSet<5>>(dim_ID_environment);

  /**
   * Create a dimension point set
   * A dimension point set defines a set of coordinate points following a dimension id set that are to be used as polar
   * coordinate sampling.
   * A dimension point set is first populated with vector of samples data for each of the dimensions. Once done, it must
   * be built to generate a cartesian product of every dimension and generate internally a vector of dimension points
   */

  // Create samples for the dimensions
  // FIXME: arange ne fonctionne vraiment pas comme voulu...
  auto STW_vector = mathutils::arange<double>(0, 20, 1);
  auto TWS_vector = mathutils::arange<double>(0, 60, 5);
  auto TWA_vector = mathutils::arange<double>(0, 180, 15);
  auto WA_vector = mathutils::arange<double>(0, 180, 15);
  auto Hs_vector = mathutils::arange<double>(0, 8, 1);

  auto dimension_point_environment = std::make_shared<DimensionPointSet<5>>(dimension_ID_environment);
  dimension_point_environment->set_dimension_values("STW_kt", STW_vector);
  dimension_point_environment->set_dimension_values("TWS_kt", TWS_vector);
  dimension_point_environment->set_dimension_values("TWA_deg", TWA_vector);
  dimension_point_environment->set_dimension_values("WA_deg", WA_vector);
  dimension_point_environment->set_dimension_values("Hs_m", Hs_vector);
//  dimension_point_environment->build();  // TODO: il faut qu'on m'envoir chier si pas built..

  /**
   * Creating attributes
   */

  Attributes attributes;
  attributes.add_attribute("polar_type", "ND");

  /**
   * Create a polar set
   *
   * A polar set allows to group several polars into one unique structure. This is really what is further used
   * to write or read on disk.
   * The polar set has attributes that are global attributes into NetCDF files
   *
   * Note that a polar is composed of a pointer to a dimension ID set and a vector of polar points.
   *
   * A polar point is composed of a pointer to a dimension point and a value. This is really a point in the polar.
   *
   * A dimension point MUST be generated from a dimension point set
   *
   * A polar is always written following the last schema specification
   *
   */

  // Overriding the newest schema for the test
  Schema newest_schema(get_json_str_from_file("schema_new.json"), true);

  Schema schema_old(get_json_str_from_file("schema_old.json"), false);

  // TODO: Ici, on veut pouvoir dire au NewestSchema quel json utiliser

//  auto polar_set = std::make_shared<PolarSet>(attributes, LastSchema::getInstance());
  auto polar_set = std::make_shared<PolarSet>(attributes, schema_old, newest_schema);

  // Adding variable
  auto brake_power = polar_set->New<double, 5>("TotalBrakePower",
                                               "kW",
                                               "Brake Power",
                                               type::POEM_TYPES::DOUBLE,
                                               dimension_point_environment);


  // Populating variable
  double val = 0.;
  for (const auto &dimension_point: *dimension_point_environment) {
    PolarPoint<double, 5> polar_point(dimension_point, val);
    brake_power->set_point(&polar_point);

    val += 1.1;
  }

  // Essai d'appel a eval direct sur polar set
//  std::array<double, 5> point = {1, 2, 3, 4, 5};
  std::cout << polar_set->eval<double, 5>("BrakePower", {1, 2, 3, 4, 5}) << std::endl;


  // Essai d'extraction de polaire
  auto brake_power_polar = polar_set->get_polar<double, 5>("BrakePower");
//  auto dim_point_set = brake_power_polar->dimension_point_set();




//  polar_set->to_netcdf("essai.nc");

  /*
   * Dans le test unitaire, on veut
   *
   * 1 - creer une polaire avec un ancien schema
   * 2 - ecrire la polaire avec cet ancien schema
   * 3 - lire la polaire ainsi creee
   * 4 - lire les variables qui ont change et tester les mappings
   *
   */



  int i = 0;

}


