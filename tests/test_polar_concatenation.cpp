//
// Created by frongere on 09/06/23.
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
  auto STW_vector = mathutils::arange<double>(0, 20, 1);
  auto TWS_vector = mathutils::arange<double>(0, 60, 5);
  auto TWA_vector = mathutils::arange<double>(0, 180, 15);
  auto WA_vector = mathutils::arange<double>(0, 180, 15);
  auto Hs_vector = mathutils::arange<double>(0, 8, 1);


  auto dimension_point_set = std::make_shared<DimensionPointSet<5>>(dimension_ID_environment);
  dimension_point_set->set_dimension_values("STW_kt", STW_vector);
  dimension_point_set->set_dimension_values("TWS_kt", TWS_vector);
  dimension_point_set->set_dimension_values("TWA_deg", TWA_vector);
  dimension_point_set->set_dimension_values("WA_deg", WA_vector);
  dimension_point_set->set_dimension_values("Hs_m", Hs_vector);
  dimension_point_set->build();

  // TODO: il faudrait avoir le generateur de dimensionPointSet a part
  auto dim_point_set_vector = dimension_point_set->split(10);


  /**
   * Creating attributes
   */

  Attributes attributes;
  attributes.add_attribute("polar_type", "ND");


  // Overriding the newest schema for the test
  Schema newest_schema(get_json_str_from_file("schema_new.json"), true);
  Schema schema_old(get_json_str_from_file("schema_old.json"), false);

  // TODO: Ici, on veut pouvoir dire au NewestSchema quel json utiliser

  std::vector<PolarSet> polar_set_vector;
  polar_set_vector.reserve(dim_point_set_vector.size());
  double val = 0.;
  for (const auto &dim_point_set: dim_point_set_vector) {
    polar_set_vector.emplace_back(attributes, schema_old, newest_schema);
    auto polar = polar_set_vector.back().New<double, 5>("TotalBrakePower",
                                                        "kW",
                                                        "Brake Power",
                                                        type::POEM_TYPES::DOUBLE,
                                                        dim_point_set);
    for (const auto &dim_point : *dim_point_set) {
      PolarPoint<double, 5> polar_point(dim_point, val);
      polar->set_point(&polar_point);
      val += 1.;
    }

  }

//  std::cout << val << std::endl;

  // Concatenation of polar
  PolarSet polar_set(polar_set_vector.front());
  for (const auto &polar_set_i : polar_set_vector) {

    if (&polar_set_i == &polar_set_vector.front()) continue;

    polar_set.append(polar_set_i);

  }







//  STOP
}