//
// Created by smichel on 02/10/2024.
//

#include <gtest/gtest.h>
#include <poem/poem.h>
#include <poem/attributes.h>

#include <MathUtils/VectorGeneration.h>

using namespace poem;

TEST(poem_polar_set, NestedForLoop) {

  std::cout << "==== Setup started " << std::endl;
  // Create Dimensions
  std::cout << "     Dimension creation " << std::endl;
  auto STW_dim = std::make_shared<poem::Dimension>("STW_kt", "kt", "Speed Through Water");
  auto TWS_dim = std::make_shared<poem::Dimension>("TWS_kt", "kt", "True Wind Speed");
  auto TWA_dim = std::make_shared<poem::Dimension>("TWA_deg", "deg", "True Wind Angle");
  auto WA_dim = std::make_shared<poem::Dimension>("WA_deg", "deg", "Waves Angle");
  auto Hs_dim = std::make_shared<poem::Dimension>("Hs_m", "m", "Waves Significant Height");

  // Create a DimensionSet
  std::cout << "     DimensionSet creation " << std::endl;
  std::array<std::shared_ptr<poem::Dimension>, 5> array{STW_dim, TWS_dim, TWA_dim, WA_dim, Hs_dim};
  auto dimension_set = std::make_shared<poem::DimensionSet<5>>(array);

  // Create samples for dimensions
  std::cout << "     Dimension samples creation " << std::endl;
  auto STW_vector = mathutils::arange<double>(0, 20, 1);
  auto TWS_vector = mathutils::arange<double>(0, 60, 5);
  auto TWA_vector = mathutils::arange<double>(0, 180, 15);
  auto WA_vector = mathutils::arange<double>(0, 180, 15);
  auto Hs_vector = mathutils::arange<double>(0, 8, 1);

  // Create the dimension grid
  std::cout << "     DimensionGrid creation " << std::endl;
  auto dimension_grid = poem::DimensionGrid(dimension_set);
  dimension_grid.set_values("STW_kt", STW_vector);
  ASSERT_FALSE(dimension_grid.is_filled());
  dimension_grid.set_values("TWS_kt", TWS_vector);
  dimension_grid.set_values("TWA_deg", TWA_vector);
  dimension_grid.set_values("WA_deg", WA_vector);
  dimension_grid.set_values("Hs_m", Hs_vector);
  ASSERT_TRUE(dimension_grid.is_filled());

  std::cout << "     DimensionPointSet creation " << std::endl;
  auto dimension_point_set = std::make_shared<poem::DimensionPointSet<5>>(dimension_grid);

  std::cout << "     Polar creation " << std::endl;
  auto polar_table = std::make_shared<poem::PolarTable<double, 5>>("BrakePower", "kW", "Brake Power",
                                                                   poem::type::POEM_TYPES::DOUBLE,
                                                                   poem::POLAR_TYPE::PPP,
                                                                   dimension_point_set);

  std::cout << "     Polar creation " << std::endl;
  poem::Attributes attributes;
  attributes.add_attribute("polar_type", "PPP");
  attributes.add_attribute("name", "ship_name_PPP");
  auto polar = std::make_shared<poem::Polar>("PPP", attributes, poem::POLAR_TYPE::PPP);
  auto p = polar->new_polar_table<double, 5>("BrakePower", "kW", "Brake Power", poem::type::POEM_TYPES::DOUBLE,
                                             dimension_point_set);
  //(polar_table, "MylittlePolar");

  std::cout << "==== Setup ended " << std::endl;

  std::cout << std::endl
            << "====================== LET'S START test_read.cpp >> PolarSetTest" << std::endl;
  std::cout << "Hola, this unit test load the Polar and read everything it can" << std::endl;

  std::cout << "- Polar name: " << attributes["polar_type"] << std::endl;//polar_table->name()
  std::cout << "  Polar type: " << polar->polar_type() << " or " << polar->polar_type_str() << std::endl;
  std::cout << "  Polar contains: ";
  auto polar_names = polar->polar_names();
  for (auto p: polar_names) {
    std::cout << p << ", ";
  }
  std::cout << std::endl;
  std::string pn = polar->polar_type_str();
  std::array<double, 5> dimension_point{10.1, 10.1, 90.1, 0, 0};
  std::cout << "  Dimension point: (";
  for (auto i: dimension_point) {
    std::cout << i << ",";
  }
  std::cout << ")" << std::endl;
  for (auto p: polar_names) {
    std::shared_ptr<poem::InterpolablePolarTable<5>> polar_table_ = polar->polar_table<double, 5>(p);
    std::cout << "  Creation of the InterpolableTable (" << p << ") - DONE " << std::endl;
    const int dim = polar_table_->dim();
    std::cout << "  Polar type: " << polar_table_->polar_type() << std::endl;
    std::cout << "  Polar dim: " << polar_table_->dim() << std::endl;
    std::cout << "  Polar size: " << polar_table_->size() << std::endl;
    std::cout << "  Polar is filled ?: " << polar_table_->is_filled() << std::endl;
    auto dim_name = polar_table_->dimension_set_names();
    for (auto n: dim_name) {
      std::cout << "    Dimension " << n;
      std::cout << " - min: " << polar_table_->min_bound(n);
      std::cout << " - max: " << polar_table_->max_bound(n) << std::endl;
    }
    std::cout << "    Min & max values of " << p;
    std::cout << " - min: " << polar_table_->min_value();
    std::cout << " - max: " << polar_table_->max_value() << std::endl;

    // auto dpl = polar_table->dimension_point_list();
    // for (auto dp: dim_set){
    //   std::cout << dp << std::endl;
    // }

    auto nr = polar_table_->nearest(dimension_point, true);
    std::cout << "  Nearest: " << nr << std::endl;
    auto interp = polar_table_->interp(dimension_point, true);
    std::cout << "  Interpolation: " << interp << std::endl;
  }
  std::cout << "Well done, PolarSetTest ended " << std::endl;
}
