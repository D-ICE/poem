//
// Created by frongere on 17/12/24.
//

#include <gtest/gtest.h>
#include <netcdf>

#include "poem/poem.h"

using namespace poem;

TEST(poem, version) {
  std::cout<< "Poem Version: " << git::GetNormalizedVersionString() << std::endl;
}


//// TODO: faire test pour chaque classe importante
//TEST(poem, DimensionSet) {
//  // TODO
//}


TEST(poem, PolarTable) {

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
  ASSERT_ANY_THROW(make_polar_table<double>("a", "unknown_to_dunits", "", POEM_DOUBLE, dimension_grid));
  ASSERT_ANY_THROW(make_polar_table<int>("b", "-", "", POEM_DOUBLE, dimension_grid));
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
  resampled_polar_table->change_name("VAR_resampled");


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


  netCDF::NcFile dataFile(std::string("polar_table.nc"), netCDF::NcFile::replace);
  write(dataFile, polar_table);
  dataFile.close();

  netCDF::NcFile dataFile_(std::string("polar_table.nc"), netCDF::NcFile::read);

  auto var = dataFile_.getVar("VAR");
  std::shared_ptr<DimensionGrid> dimension_grid_read;
  auto polar_table_read = read(var, dimension_grid_read, true);
  dataFile_.close();

  ASSERT_EQ(*polar_table, dynamic_cast<PolarTable<double>&>(*polar_table_read));

//  // Polar
//  auto polar = Polar("polar", MPPP, dimension_grid);
//  polar.add_polar_table(polar_table);
//  auto new_polar_table = polar.new_polar_table<int>("VAR2", "-", "VAR2", POEM_DOUBLE);
//
//  auto new_polar_table_back = polar.polar("VAR2");
//  auto type = new_polar_table_back->type();

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

}

TEST(poem, Polar) {
  // Dimensions
  auto dim1 = make_dimension("dim1", "-", "dim1");
  auto dim2 = make_dimension("dim2", "-", "dim2");
  auto dim3 = make_dimension("dim3", "-", "dim3");

  // DimensionSet
  auto dimension_set = make_dimension_set({dim1, dim2, dim3});

  // DimensionGrid
  auto dimension_grid = make_dimension_grid(dimension_set);
  dimension_grid->set_values("dim1", {1, 2, 3, 4, 5});
  dimension_grid->set_values("dim2", {1, 2, 3, 4, 5});
  dimension_grid->set_values("dim3", {1, 2, 3, 4, 5});

  // Polar
  auto polar = make_polar("MPPP", MPPP, dimension_grid);

  // PolarTables
  auto table_double = polar->new_polar_table<double>("table_double", "-", "table_double", POEM_DOUBLE);
  table_double->fill_with(1.);
  auto table_int = polar->new_polar_table<int>("table_int", "-", "table_int", POEM_INT);
  table_int->fill_with(2);

  // Writing
  netCDF::NcFile dataFile(std::string("polar.nc"), netCDF::NcFile::replace);
  write(dataFile, polar);
  dataFile.close();

  // Reading back
  netCDF::NcFile dataFile_(std::string("polar.nc"), netCDF::NcFile::read);
  auto polar_ = read(dataFile_);

  ASSERT_EQ(*polar, *polar_);

}


TEST(poem, OperationMode) {

  // TODO: voir si dans dtree on fait apparaitre le nom de root... /root
  auto root = std::make_shared<OperationMode>("vessel_name");
  ASSERT_TRUE(root->is_root());

  auto ballast_load = root->new_child<OperationMode>("ballast_load");
  auto ballast_one_engine = ballast_load->new_child<OperationMode>("ballast_one_engine");
  auto ballast_two_engines = ballast_load->new_child<OperationMode>("ballast_two_engines");

  auto laden_load = root->new_child<OperationMode>("laden_load");
  auto laden_one_engine = laden_load->new_child<OperationMode>("laden_one_engine");
  auto laden_two_engines = laden_load->new_child<OperationMode>("laden_two_engines");

  // root->ballast_load->ballast_one_engine
  //                   ->ballast_two_engines
  //     ->laden_load->laden_one_engine
  //                 ->laden_two_engines


  ASSERT_ANY_THROW(laden_load->polar_set());

  auto STW = make_dimension("STW", "kt", "");
  auto TWS = make_dimension("TWS", "kt", "");
  auto TWA = make_dimension("TWA", "deg", "");
  auto WA = make_dimension("WA", "deg", "");
  auto Hs = make_dimension("Hs", "m", "");


  auto dimension_set_5D = make_dimension_set({STW, TWS, TWA, WA, Hs});
  auto dimension_grid_5D = make_dimension_grid(dimension_set_5D);
  dimension_grid_5D->set_values("STW", {1, 2, 3});
  dimension_grid_5D->set_values("TWS", {1, 2, 3});
  dimension_grid_5D->set_values("TWA", {1, 2, 3});
  dimension_grid_5D->set_values("WA", {1, 2, 3});
  dimension_grid_5D->set_values("Hs", {1, 2, 3});

  auto dimension_set_4D = make_dimension_set({TWS, TWA, WA, Hs});
  auto dimension_grid_4D = make_dimension_grid(dimension_set_4D);
  dimension_grid_4D->set_values("TWS", {1, 2, 3});
  dimension_grid_4D->set_values("TWA", {1, 2, 3});
  dimension_grid_4D->set_values("WA", {1, 2, 3});
  dimension_grid_4D->set_values("Hs", {1, 2, 3});

  auto polar_set = laden_two_engines->polar_set();
  auto polar_MPPP = polar_set->create_polar(MPPP, dimension_grid_5D);
  auto MPPP_double = polar_MPPP->new_polar_table<double>("Double", "kW", "double", POEM_DOUBLE);
  MPPP_double->fill_with(10.);
  auto MPPP_int = polar_MPPP->new_polar_table<int>("Int", "-", "int", POEM_INT);
  MPPP_int->fill_with(3);

  auto polar_HPPP = polar_set->create_polar(HPPP, dimension_grid_5D);
  auto polar_MVPP = polar_set->create_polar(MVPP, dimension_grid_5D);
  auto polar_HVPP = polar_set->create_polar(HVPP, dimension_grid_5D);
  auto polar_VPP = polar_set->create_polar(VPP, dimension_grid_4D);

  std::cout << polar_MPPP->name() << std::endl;

}
