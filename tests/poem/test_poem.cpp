//
// Created by frongere on 17/12/24.
//

#include <gtest/gtest.h>
#include <netcdf>

#include <MathUtils/VectorGeneration.h>

#include "poem/poem.h"

using namespace poem;

TEST(poem, version) {
  std::cout << "Poem Version: " << git::GetNormalizedVersionString() << std::endl;
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


  netCDF::NcFile data_file(std::string("polar_table.nc"), netCDF::NcFile::replace);
  to_netcdf(polar_table, data_file);
//  polar_table->to_netcdf(data_file);
//  write_polar_table(dataFile, polar_table);
  data_file.close();

  netCDF::NcFile dataFile_(std::string("polar_table.nc"), netCDF::NcFile::read);

  auto var = dataFile_.getVar("VAR");
  std::shared_ptr<DimensionGrid> dimension_grid_read;
  auto polar_table_read = read_polar_table(var, dimension_grid_read, true);
  dataFile_.close();

  ASSERT_EQ(*polar_table, dynamic_cast<PolarTable<double> &>(*polar_table_read));

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
  auto table_double = polar->create_polar_table<double>("table_double", "-", "table_double", POEM_DOUBLE);
  table_double->fill_with(1.);
  auto table_int = polar->create_polar_table<int>("table_int", "-", "table_int", POEM_INT);
  table_int->fill_with(2);

  // Writing
  netCDF::NcFile dataFile(std::string("polar.nc"), netCDF::NcFile::replace);
  write_polar(polar, dataFile);
  dataFile.close();

  // Reading back
  netCDF::NcFile dataFile_(std::string("polar.nc"), netCDF::NcFile::read);
  auto polar_ = read_polar(dataFile_);

  ASSERT_EQ(*polar, *polar_);

  ASSERT_EQ(polar->name(), "MPPP");

}

TEST(poem, PolarSet) {
  // Dimensions
  auto STW = make_dimension("STW", "kt", "Speed Through Water");
  auto TWS = make_dimension("TWS", "kt", "True Wind Speed");
  auto TWA = make_dimension("TWA", "deg", "True Wind Angle");
  auto WA = make_dimension("WA", "deg", "Waves Angle");
  auto Hs = make_dimension("Hs", "m", "Waves Significant Height");
  auto BrakePower = make_dimension("BrakePower", "kW", "Brake Power");

  // DimensionSet
  auto dimension_set_speed_control = make_dimension_set({STW, TWS, TWA, WA, Hs});
  auto dimension_set_power_control = make_dimension_set({BrakePower, TWS, TWA, WA, Hs});
  auto dimension_set_no_control = make_dimension_set({TWS, TWA, WA, Hs});

  // DimensionGrid
  auto STW_values = mathutils::linspace<double>(8, 20, 13);
  auto TWS_values = mathutils::linspace<double>(0, 40, 9);
  auto TWA_values = mathutils::linspace<double>(0, 180, 13);
  auto WA_values = mathutils::linspace<double>(0, 180, 13);
  auto Hs_values = mathutils::linspace<double>(0, 8, 9);
  auto BrakePower_values = mathutils::linspace<double>(1000, 6500, 12);

  auto dimension_grid_speed_control = make_dimension_grid(dimension_set_speed_control);
  dimension_grid_speed_control->set_values("STW", STW_values);
  dimension_grid_speed_control->set_values("TWS", TWS_values);
  dimension_grid_speed_control->set_values("TWA", TWA_values);
  dimension_grid_speed_control->set_values("WA", WA_values);
  dimension_grid_speed_control->set_values("Hs", Hs_values);

  auto dimension_grid_power_control = make_dimension_grid(dimension_set_power_control);
  dimension_grid_power_control->set_values("BrakePower", BrakePower_values);
  dimension_grid_power_control->set_values("TWS", TWS_values);
  dimension_grid_power_control->set_values("TWA", TWA_values);
  dimension_grid_power_control->set_values("WA", WA_values);
  dimension_grid_power_control->set_values("Hs", Hs_values);

  auto dimension_grid_no_control = make_dimension_grid(dimension_set_no_control);
  dimension_grid_no_control->set_values("TWS", TWS_values);
  dimension_grid_no_control->set_values("TWA", TWA_values);
  dimension_grid_no_control->set_values("WA", WA_values);
  dimension_grid_no_control->set_values("Hs", Hs_values);

  // PolarSet
  auto polar_set = make_polar_set("as_polar_set");
  polar_set->create_polar(MPPP, dimension_grid_speed_control);

  ASSERT_EQ(polar_set->polar(MPPP)->name(), "MPPP");
  ASSERT_EQ(polar_set->polar(MPPP)->full_name(), "/as_polar_set/MPPP");

  polar_set->create_polar(HPPP, dimension_grid_speed_control);
  ASSERT_EQ(polar_set->polar(HPPP)->name(), "HPPP");
  ASSERT_EQ(polar_set->polar(HPPP)->full_name(), "/as_polar_set/HPPP");

  polar_set->create_polar(MVPP, dimension_grid_power_control);
  ASSERT_EQ(polar_set->polar(MVPP)->name(), "MVPP");
  ASSERT_EQ(polar_set->polar(MVPP)->full_name(), "/as_polar_set/MVPP");

  polar_set->create_polar(HVPP, dimension_grid_power_control);
  ASSERT_EQ(polar_set->polar(HVPP)->name(), "HVPP");
  ASSERT_EQ(polar_set->polar(HVPP)->full_name(), "/as_polar_set/HVPP");

  polar_set->create_polar(VPP, dimension_grid_no_control);
  ASSERT_EQ(polar_set->polar(VPP)->name(), "VPP");
  ASSERT_EQ(polar_set->polar(VPP)->full_name(), "/as_polar_set/VPP");

  // Create some tables into the different Polar
  polar_set->polar(MPPP)->create_polar_table<double>("BrakePower", "kW", "BrakePower", POEM_DOUBLE)->fill_with(1000.);
  polar_set->polar(MPPP)->create_polar_table<int>("SolverStatus", "-", "Solver Status", POEM_INT)->fill_with(1);

  polar_set->polar(HPPP)->create_polar_table<double>("BrakePower", "kW", "BrakePower", POEM_DOUBLE)->fill_with(1000.);
  polar_set->polar(HPPP)->create_polar_table<int>("SolverStatus", "-", "Solver Status", POEM_INT)->fill_with(1);

  polar_set->polar(MVPP)->create_polar_table<double>("STW", "kt", "Speed Through Water", POEM_DOUBLE)->fill_with(10.);
  polar_set->polar(MVPP)->create_polar_table<int>("SolverStatus", "-", "Solver Status", POEM_INT)->fill_with(1);

  polar_set->polar(HVPP)->create_polar_table<double>("STW", "kt", "Speed Through Water", POEM_DOUBLE)->fill_with(10.);
  polar_set->polar(HVPP)->create_polar_table<int>("SolverStatus", "-", "Solver Status", POEM_INT)->fill_with(1);

  polar_set->polar(VPP)->create_polar_table<double>("STW", "kt", "Speed Through Water", POEM_DOUBLE)->fill_with(10.);
  polar_set->polar(VPP)->create_polar_table<int>("SolverStatus", "-", "Solver Status", POEM_INT)->fill_with(1);

  // Writing
  netCDF::NcFile dataFile(std::string("as_polar_set.nc"), netCDF::NcFile::replace);
  write_polar_set(polar_set, dataFile);
  dataFile.close();

  // Reading back
  netCDF::NcFile dataFile_(std::string("as_polar_set.nc"), netCDF::NcFile::read);
  auto polar_set_ = read_polar_set(dataFile_);

  ASSERT_EQ(*polar_set, *polar_set_);

}

void fill(std::shared_ptr<PolarSet> polar_set) {

  // Dimensions
  auto STW = make_dimension("STW", "kt", "Speed Through Water");
  auto TWS = make_dimension("TWS", "kt", "True Wind Speed");
  auto TWA = make_dimension("TWA", "deg", "True Wind Angle");
  auto WA = make_dimension("WA", "deg", "Waves Angle");
  auto Hs = make_dimension("Hs", "m", "Waves Significant Height");
  auto BrakePower = make_dimension("BrakePower", "kW", "Brake Power");

  // DimensionSet
  auto dimension_set_speed_control = make_dimension_set({STW, TWS, TWA, WA, Hs});
  auto dimension_set_power_control = make_dimension_set({BrakePower, TWS, TWA, WA, Hs});
  auto dimension_set_no_control = make_dimension_set({TWS, TWA, WA, Hs});

  // DimensionGrid
  auto STW_values = mathutils::linspace<double>(8, 20, 13);
  auto TWS_values = mathutils::linspace<double>(0, 40, 9);
  auto TWA_values = mathutils::linspace<double>(0, 180, 13);
  auto WA_values = mathutils::linspace<double>(0, 180, 13);
  auto Hs_values = mathutils::linspace<double>(0, 8, 9);
  auto BrakePower_values = mathutils::linspace<double>(1000, 6500, 12);

  auto dimension_grid_speed_control = make_dimension_grid(dimension_set_speed_control);
  dimension_grid_speed_control->set_values("STW", STW_values);
  dimension_grid_speed_control->set_values("TWS", TWS_values);
  dimension_grid_speed_control->set_values("TWA", TWA_values);
  dimension_grid_speed_control->set_values("WA", WA_values);
  dimension_grid_speed_control->set_values("Hs", Hs_values);

  auto dimension_grid_power_control = make_dimension_grid(dimension_set_power_control);
  dimension_grid_power_control->set_values("BrakePower", BrakePower_values);
  dimension_grid_power_control->set_values("TWS", TWS_values);
  dimension_grid_power_control->set_values("TWA", TWA_values);
  dimension_grid_power_control->set_values("WA", WA_values);
  dimension_grid_power_control->set_values("Hs", Hs_values);

  auto dimension_grid_no_control = make_dimension_grid(dimension_set_no_control);
  dimension_grid_no_control->set_values("TWS", TWS_values);
  dimension_grid_no_control->set_values("TWA", TWA_values);
  dimension_grid_no_control->set_values("WA", WA_values);
  dimension_grid_no_control->set_values("Hs", Hs_values);

// Create some tables into the different Polar
  polar_set->create_polar(MPPP, dimension_grid_speed_control);
  polar_set->polar(MPPP)->create_polar_table<double>("BrakePower", "kW", "BrakePower", POEM_DOUBLE)->fill_with(1000.);
  polar_set->polar(MPPP)->create_polar_table<int>("SolverStatus", "-", "Solver Status", POEM_INT)->fill_with(1);


  polar_set->create_polar(HPPP, dimension_grid_speed_control);
  polar_set->polar(HPPP)->create_polar_table<double>("BrakePower", "kW", "BrakePower", POEM_DOUBLE)->fill_with(1000.);
  polar_set->polar(HPPP)->create_polar_table<int>("SolverStatus", "-", "Solver Status", POEM_INT)->fill_with(1);

  polar_set->create_polar(MVPP, dimension_grid_power_control);
  polar_set->polar(MVPP)->create_polar_table<double>("STW", "kt", "Speed Through Water", POEM_DOUBLE)->fill_with(10.);
  polar_set->polar(MVPP)->create_polar_table<int>("SolverStatus", "-", "Solver Status", POEM_INT)->fill_with(1);

  polar_set->create_polar(HVPP, dimension_grid_power_control);
  polar_set->polar(HVPP)->create_polar_table<double>("STW", "kt", "Speed Through Water", POEM_DOUBLE)->fill_with(10.);
  polar_set->polar(HVPP)->create_polar_table<int>("SolverStatus", "-", "Solver Status", POEM_INT)->fill_with(1);

  polar_set->create_polar(VPP, dimension_grid_no_control);
  polar_set->polar(VPP)->create_polar_table<double>("STW", "kt", "Speed Through Water", POEM_DOUBLE)->fill_with(10.);
  polar_set->polar(VPP)->create_polar_table<int>("SolverStatus", "-", "Solver Status", POEM_INT)->fill_with(1);

}


TEST(poem, PolarNode) {


  auto vessel = std::make_shared<PolarNode>("vessel");
  ASSERT_TRUE(vessel->is_root());

  auto ballast_load = vessel->new_child<PolarNode>("ballast_load");
  auto ballast_one_engine = ballast_load->new_child<PolarSet>("ballast_one_engine");
  auto ballast_two_engines = ballast_load->new_child<PolarSet>("ballast_two_engines");

  auto laden_load = vessel->new_child<PolarNode>("laden_load");
  auto laden_one_engine = laden_load->new_child<PolarSet>("laden_one_engine");
  auto laden_two_engines = laden_load->new_child<PolarSet>("laden_two_engines");


  // vessel_name ->ballast_load -> ballast_one_engine
  //                            -> ballast_two_engines
  //             ->laden_load   -> laden_one_engine
  //                            ->laden_two_engines

  ASSERT_EQ(vessel->name(), "vessel");
  ASSERT_EQ(vessel->full_name(), "/vessel");
  ASSERT_EQ(ballast_load->name(), "ballast_load");
  ASSERT_EQ(ballast_load->full_name(), "/vessel/ballast_load");
  ASSERT_EQ(ballast_one_engine->name(), "ballast_one_engine");
  ASSERT_EQ(ballast_one_engine->full_name(), "/vessel/ballast_load/ballast_one_engine");

  ASSERT_ANY_THROW(laden_load->as_polar_set());
  ASSERT_ANY_THROW(laden_load->as_polar_table());

  fill(ballast_one_engine);
  fill(ballast_two_engines);
  fill(laden_one_engine);
  fill(laden_two_engines);

  ASSERT_EQ(ballast_one_engine->polar(MPPP)->name(), "MPPP");
  ASSERT_EQ(ballast_one_engine->polar(MPPP)->full_name(), "/vessel/ballast_load/ballast_one_engine/MPPP");
  ASSERT_EQ(ballast_one_engine->polar(MPPP)->polar_table("BrakePower")->full_name(),
            "/vessel/ballast_load/ballast_one_engine/MPPP/BrakePower");


  // Writing
  to_netcdf(ballast_one_engine, "ballast_one_engine.nc");

//  // Reading back
//  netCDF::NcFile dataFile_ballast_one_engine_(std::string("ballast_one_engine.nc"), netCDF::NcFile::read);
//  auto ballast_one_engine_ = read_operation_mode(dataFile_ballast_one_engine_);
//
//  ASSERT_EQ(*ballast_one_engine, *ballast_one_engine_);

  // Writing
  to_netcdf(vessel, "vessel.nc");

//  // Reading back
//  netCDF::NcFile dataFile_root_(std::string("vessel.nc"), netCDF::NcFile::read);
//  auto root_ = read_operation_mode(dataFile_root_);
//
//  ASSERT_EQ(*root, *root_);

}

TEST(poem, read_poem_v0_example) {

  ASSERT_ANY_THROW(read_poem_nc_file("dont_exist.nc"));

  auto vessel = read_poem_nc_file("poem_v0_example_no_sails.nc");

  to_netcdf(vessel, "poem_v0_example_no_sails_v0_to_v1.nc");

//  read_poem_nc_file("poem_v0_example_no_sails_v0_to_v1.nc");

}
