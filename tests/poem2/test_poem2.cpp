//
// Created by frongere on 17/12/24.
//

#include <gtest/gtest.h>
#include <poem2/poem2.h>

using namespace poem2;

// TODO: faire test pour chaque classe importante
TEST(poem2, DimensionSet) {
  // TODO
}


TEST(poem2, polar_table) {

  // Dimensions
  auto STW = make_dimension("STW", "kt", "Speed Through Water");
  auto TWS = make_dimension("TWS", "kt", "True Wind Speed");
  auto TWA = make_dimension("TWA", "deg", "True Wind Angle");

  // DimensionSet
  auto dimension_set = make_dimension_set({STW, TWS, TWA});

  // DimensionGrid
  auto dimension_grid = make_dimension_grid(dimension_set);
  dimension_grid->set_values("TWA", {1, 2, 3});

  ASSERT_ANY_THROW(dimension_grid->dimension_points());

  dimension_grid->set_values("STW", {1, 2, 3});
  dimension_grid->set_values("TWS", {1, 2, 3});


  ASSERT_NO_THROW(dimension_grid->dimension_points());
//  auto dimension_points = dimension_grid->dimension_points();


  // PolarTable
  auto polar_table = make_polar_table<double>("VAR", "-", "VAR", POEM_DOUBLE, dimension_grid);
//  auto polar = std::make_shared<PolarTable<double>>(dimension_grid);

  auto dimension_points = polar_table->dimension_points();

  // Filling the polar with values
  size_t idx = 0;
  for (const auto &dimension_point: polar_table->dimension_points()) {
    double val = 1.;
    for (const auto coord: dimension_point) {
      val *= coord;
    }

    polar_table->set_value(idx, val);
    idx++;
  }

  // Interpolation
  DimensionPoint dimension_point(dimension_set, {1.2, 1.9, 2.8});

  auto val_interp = polar_table->interp(dimension_point, true);
  double val_calc = 1.2 * 1.9 * 2.8;


  auto shape = polar_table->shape();

  // Slicing
  std::unordered_map<std::string, double> slice{{"STW", 1.2},
                                                {"TWS", 2.05}};

  auto sliced_polar_table = polar_table->slice(slice);

  // <Check that the sliced Polar is ok
  for (const auto &dimension_point_: sliced_polar_table->dimension_points()) {
    double val_calculated = 1.;
    for (const auto &coord: dimension_point_) {
      val_calculated *= coord;
    }
    double val_from_sliced_polar = sliced_polar_table->interp(dimension_point_, false);

    ASSERT_EQ(val_from_sliced_polar, val_calculated);
  }

  auto sliced_shape = sliced_polar_table->shape();

  sliced_polar_table->squeeze();

  // Copy
  auto polar_table_copy = polar_table->copy();

  // Resampling
  auto new_dimension_grid = polar_table->dimension_grid()->copy();
  new_dimension_grid->set_values("STW", {1, 1.5, 2, 2.5, 3});
  auto resampled_polar_table = polar_table->resample(new_dimension_grid);
  resampled_polar_table->rename("VAR_resampled");


  for (const auto &dimension_point_: resampled_polar_table->dimension_points()) {
    double val_calculated = 1.;
    for (const auto &coord: dimension_point_) {
      val_calculated *= coord;
    }
    double val_from_polar = resampled_polar_table->interp(dimension_point_, false);

    ASSERT_EQ(val_from_polar, val_calculated);
  }

  shape = resampled_polar_table->shape();

  // grid to index
  ASSERT_EQ(polar_table->dimension_grid()->grid_to_index({0, 1, 2}), 5);

  // Nearest
  dimension_point = {1.2, 2.1, 2.8};
  auto nearest_val = polar_table->nearest(dimension_point);
  ASSERT_EQ(nearest_val, 6.);


  polar_table->sum(polar_table);
//  polar_table->sum(polar_table);


  netCDF::NcFile dataFile(std::string("write_polar_table.nc"), netCDF::NcFile::replace);
  write(dataFile, polar_table);
  dataFile.close();

  netCDF::NcFile dataFile_(std::string("write_polar_table.nc"), netCDF::NcFile::read);
//  auto dimension_grid_ = read_dimension_grid(dataFile_);

  auto var = dataFile_.getVar("VAR");
  std::shared_ptr<DimensionGrid> dimension_grid_read;
  auto polar_table_read = read(var, dimension_grid_read, true);
  dataFile_.close();

  for(size_t i=0; i<polar_table->size(); ++i) {
    ASSERT_EQ(polar_table->values()[i], dynamic_cast<PolarTable<double>*>(polar_table_read.get())->values()[i]);
  }


  // Polar
  auto polar = Polar("polar", MPPP, dimension_grid);
  polar.add_polar(polar_table);
  auto new_polar_table = polar.new_polar_table<int>("VAR2", "-", "VAR2", POEM_DOUBLE);

  auto new_polar_table_back = polar.polar("VAR2");
  auto type = new_polar_table_back->type();

  /*
   *  Slice ok
   * Squeeeze ok
   * Nearest ok
   * mean ok
   * Intégration dans Polar & PolarSet
   * to_netcdf ok
   * read ok
   *
   * Remise en place des mutexes
   *
   */

  return;
}

TEST(poem2, PolarTree) {






  return;
}
