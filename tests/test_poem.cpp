//
// Created by frongere on 19/07/23.
//

#include <gtest/gtest.h>
#include <poem/poem.h>

#include <MathUtils/VectorGeneration.h>

class DimensionPointSetTest : public testing::Test {
 protected:
  void SetUp() override {

    // Create Dimensions
    auto STW_dim = std::make_shared<poem::Dimension>("STW_kt", "kt", "Speed Through Water");
    auto TWS_dim = std::make_shared<poem::Dimension>("TWS_kt", "kt", "True Wind Speed");
    auto TWA_dim = std::make_shared<poem::Dimension>("TWA_deg", "deg", "True Wind Angle");
    auto WA_dim = std::make_shared<poem::Dimension>("WA_deg", "deg", "Waves Angle");
    auto Hs_dim = std::make_shared<poem::Dimension>("Hs_m", "m", "Waves Significant Height");

    // Create a DimensionSet
    std::array<std::shared_ptr<poem::Dimension>, 5> array{STW_dim, TWS_dim, TWA_dim, WA_dim, Hs_dim};
    auto dimension_set = std::make_shared<poem::DimensionSet<5>>(array);

    // Create samples for dimensions
    STW_vector = mathutils::arange<double>(0, 20, 1);
    TWS_vector = mathutils::arange<double>(0, 60, 5);
    TWA_vector = mathutils::arange<double>(0, 180, 15);
    WA_vector = mathutils::arange<double>(0, 180, 15);
    Hs_vector = mathutils::arange<double>(0, 8, 1);

    // Create the dimension grid
    auto dimension_grid = poem::DimensionGrid(dimension_set);
    dimension_grid.set_values("STW_kt", STW_vector);
    ASSERT_FALSE(dimension_grid.is_filled());
    dimension_grid.set_values("TWS_kt", TWS_vector);
    dimension_grid.set_values("TWA_deg", TWA_vector);
    dimension_grid.set_values("WA_deg", WA_vector);
    dimension_grid.set_values("Hs_m", Hs_vector);
    ASSERT_TRUE(dimension_grid.is_filled());

    dimension_point_set = std::make_shared<poem::DimensionPointSet<5>>(dimension_grid);

    auto current_schema = poem::get_newest_schema();

    polar_set = std::make_shared<poem::PolarSet>();

    polar_set->new_polar<double, 5>("BrakePower", "kW", "Brake Power",
                                    poem::type::DOUBLE, poem::PPP, dimension_point_set);
  }

  std::vector<double> STW_vector;
  std::vector<double> TWS_vector;
  std::vector<double> TWA_vector;
  std::vector<double> WA_vector;
  std::vector<double> Hs_vector;
  std::shared_ptr<poem::DimensionPointSet<5>> dimension_point_set;
  std::shared_ptr<poem::PolarSet> polar_set;

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

TEST(poem_splitter, splitter) {

  size_t chunk_size = 5;

  poem::Splitter splitter1(1, chunk_size);
  ASSERT_EQ(splitter1.nchunks(), 1);
  ASSERT_EQ(splitter1.chunk_offset(0), 0);
  ASSERT_EQ(splitter1.chunk_size(0), 1);

  poem::Splitter splitter4(4, chunk_size);
  ASSERT_EQ(splitter4.nchunks(), 1);
  ASSERT_EQ(splitter4.chunk_offset(0), 0);
  ASSERT_EQ(splitter4.chunk_size(0), 4);

  poem::Splitter splitter8(8, chunk_size);
  ASSERT_EQ(splitter8.nchunks(), 1);
  ASSERT_EQ(splitter8.chunk_size(0), 8);

  poem::Splitter splitter10(10, chunk_size);
  ASSERT_EQ(splitter10.nchunks(), 2);
  ASSERT_EQ(splitter10.chunk_size(0), 5);
  ASSERT_EQ(splitter10.chunk_offset(1), 5);
  ASSERT_EQ(splitter10.chunk_size(1), 5);

  poem::Splitter splitter11(11, chunk_size);
  ASSERT_EQ(splitter11.nchunks(), 2);
  ASSERT_EQ(splitter11.chunk_size(0), 6);
  ASSERT_EQ(splitter11.chunk_offset(1), 6);
  ASSERT_EQ(splitter11.chunk_size(1), 5);

  poem::Splitter splitter12(12, chunk_size); // ok
  ASSERT_EQ(splitter12.nchunks(), 2);
  ASSERT_EQ(splitter12.chunk_size(0), 6);
  ASSERT_EQ(splitter12.chunk_offset(1), 6);
  ASSERT_EQ(splitter12.chunk_size(1), 6);

  poem::Splitter splitter13(13, chunk_size);
  ASSERT_EQ(splitter13.nchunks(), 2);
  ASSERT_EQ(splitter13.chunk_size(0), 7);
  ASSERT_EQ(splitter13.chunk_offset(1), 7);
  ASSERT_EQ(splitter13.chunk_size(1), 6);

  poem::Splitter splitter14(14, chunk_size); // ok
  ASSERT_EQ(splitter14.nchunks(), 2);
  ASSERT_EQ(splitter14.chunk_size(0), 7);
  ASSERT_EQ(splitter14.chunk_offset(1), 7);
  ASSERT_EQ(splitter14.chunk_size(1), 7);

  poem::Splitter splitter15(15, chunk_size); // ok
  ASSERT_EQ(splitter15.nchunks(), 3);
  ASSERT_EQ(splitter15.chunk_size(0), 5);
  ASSERT_EQ(splitter15.chunk_offset(1), 5);
  ASSERT_EQ(splitter15.chunk_size(1), 5);
  ASSERT_EQ(splitter15.chunk_offset(2), 10);
  ASSERT_EQ(splitter15.chunk_size(2), 5);

  poem::Splitter splitter19(19, chunk_size);
  ASSERT_EQ(splitter19.nchunks(), 3);
  ASSERT_EQ(splitter19.chunk_size(0), 7);
  ASSERT_EQ(splitter19.chunk_offset(1), 7);
  ASSERT_EQ(splitter19.chunk_size(1), 6);
  ASSERT_EQ(splitter19.chunk_offset(2), 13);
  ASSERT_EQ(splitter19.chunk_size(2), 6);

  poem::Splitter splitter20(20, chunk_size);
  ASSERT_EQ(splitter20.nchunks(), 4);
  ASSERT_EQ(splitter20.chunk_size(0), 5);
  ASSERT_EQ(splitter20.chunk_offset(1), 5);
  ASSERT_EQ(splitter20.chunk_size(1), 5);
  ASSERT_EQ(splitter20.chunk_offset(2), 10);
  ASSERT_EQ(splitter20.chunk_size(2), 5);
  ASSERT_EQ(splitter20.chunk_offset(3), 15);
  ASSERT_EQ(splitter20.chunk_size(3), 5);

  poem::Splitter splitter22(22, chunk_size);
  ASSERT_EQ(splitter22.nchunks(), 4);
  ASSERT_EQ(splitter22.chunk_size(0), 6);
  ASSERT_EQ(splitter22.chunk_offset(1), 6);
  ASSERT_EQ(splitter22.chunk_size(1), 6);
  ASSERT_EQ(splitter22.chunk_offset(2), 12);
  ASSERT_EQ(splitter22.chunk_size(2), 5);
  ASSERT_EQ(splitter22.chunk_offset(3), 17);
  ASSERT_EQ(splitter22.chunk_size(3), 5);

  poem::Splitter splitter27(27, chunk_size);
  ASSERT_EQ(splitter27.nchunks(), 5);
  ASSERT_EQ(splitter27.chunk_size(0), 6);
  ASSERT_EQ(splitter27.chunk_offset(1), 6);
  ASSERT_EQ(splitter27.chunk_size(1), 6);
  ASSERT_EQ(splitter27.chunk_offset(2), 12);
  ASSERT_EQ(splitter27.chunk_size(2), 5);
  ASSERT_EQ(splitter27.chunk_offset(3), 17);
  ASSERT_EQ(splitter27.chunk_size(3), 5);
  ASSERT_EQ(splitter27.chunk_offset(4), 22);
  ASSERT_EQ(splitter27.chunk_size(4), 5);

}
