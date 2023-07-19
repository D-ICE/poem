//
// Created by frongere on 19/07/23.
//

#include <gtest/gtest.h>
#include <poem/poem.h>

#include <MathUtils/VectorGeneration.h>

//using namespace poem;

class DimensionPointSetTest : public testing::Test {
 protected:
  void SetUp() override {

    // Create samples for dimensions
    STW_vector = mathutils::arange<double>(0, 20, 1);
    TWS_vector = mathutils::arange<double>(0, 60, 5);
    TWA_vector = mathutils::arange<double>(0, 180, 15);
    WA_vector = mathutils::arange<double>(0, 180, 15);
    Hs_vector = mathutils::arange<double>(0, 8, 1);

    // Create Dimensions
    auto STW_dim = std::make_shared<poem::Dimension>("STW_kt", "kt", "Speed Through Water", STW_vector);
    auto TWS_dim = std::make_shared<poem::Dimension>("TWS_kt", "kt", "True Wind Speed", TWS_vector);
    auto TWA_dim = std::make_shared<poem::Dimension>("TWA_deg", "deg", "True Wind Angle", TWA_vector);
    auto WA_dim = std::make_shared<poem::Dimension>("WA_deg", "deg", "Waves Angle", WA_vector);
    auto Hs_dim = std::make_shared<poem::Dimension>("Hs_m", "m", "Waves Significant Height", Hs_vector);

    // Create a DimensionSet
    poem::DimensionArray<5> dimension_array{{STW_dim, TWS_dim, TWA_dim, WA_dim, Hs_dim}};

    dimension_point_set = std::make_shared<poem::DimensionPointSet<5>>(dimension_array);
  }

  std::vector<double> STW_vector;
  std::vector<double> TWS_vector;
  std::vector<double> TWA_vector;
  std::vector<double> WA_vector;
  std::vector<double> Hs_vector;
  std::shared_ptr<poem::DimensionPointSet<5>> dimension_point_set;


};


TEST_F(DimensionPointSetTest, NestedForLoop) {

  // Testing the nested for loop
  size_t idx = 0;
  for (const auto &STW: STW_vector) {
    for (const auto &TWS: TWS_vector) {
      for (const auto &TWA: TWA_vector) {
        for (const auto &WA: WA_vector) {
          for (const auto &Hs: Hs_vector) {
            auto dimension_point = dimension_point_set->dimension_point(idx);
            ASSERT_DOUBLE_EQ(dimension_point["STW_kt"], STW);
            ASSERT_DOUBLE_EQ(dimension_point["TWS_kt"], TWS);
            ASSERT_DOUBLE_EQ(dimension_point["TWA_deg"], TWA);
            ASSERT_DOUBLE_EQ(dimension_point["WA_deg"], WA);
            ASSERT_DOUBLE_EQ(dimension_point["Hs_m"], Hs);
            idx++;
          }
        }
      }
    }
  }

}

TEST_F(DimensionPointSetTest, PolarSetTest) {

  auto current_schema = poem::get_newest_schema();

  auto polar_set = std::make_shared<poem::PolarSet>(current_schema, current_schema);

  polar_set->new_polar<double, 5>("BrakePower", "kW", "Brake Power", poem::type::DOUBLE, dimension_point_set);
  polar_set->new_polar<double, 5>("BrakePower", "kW", "Brake Power", poem::type::DOUBLE, dimension_point_set);


}

TEST_F(DimensionPointSetTest, PolarSetHermesTest) {


}

//class PolarSetTest : public DimensionPointSetTest {
// protected:
//
//  void SetUp() override {
//    DimensionPointSetTest::SetUp();
//  }
//};


