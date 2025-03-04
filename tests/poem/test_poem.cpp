//
// Created by frongere on 17/12/24.
//

#include <gtest/gtest.h>
#include <netcdf>
#include <fstream>

#include <MathUtils/VectorGeneration.h>

#include "poem/poem.h"

using namespace poem;

TEST(poem, version) {
  std::cout << "IS DIRTY: " << git::is_dirty() << std::endl;
  std::cout << "FULL VERSION: " << git::version_full() << std::endl;
  std::cout << "LAST TAG: " << git::last_tag() << std::endl;
  std::cout << "LAST COMMIT DATE: " << git::last_commit_date() << std::endl;
  std::cout << "CURRENT BRANCH: " << git::current_branch() << std::endl;

  std::cout << "MAJOR: " << git::version_major() << std::endl;
  std::cout << "MINOR: " << git::version_minor() << std::endl;
  std::cout << "PATCH: " << git::version_patch() << std::endl;

  std::cout << "CURRENT POEM SPEC VERSION: " << current_poem_standard_version() << std::endl;

}

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

  // PolarTable
  auto polar_table_double = make_polar_table<double>("VAR", "-", "VAR", POEM_DOUBLE, dimension_grid);
  auto polar_table_int = make_polar_table<int>("VAR_INT", "-", "in PolarTable", POEM_INT, dimension_grid);

  ASSERT_ANY_THROW(make_polar_table<double>("a", "unknown_to_dunits", "", POEM_DOUBLE, dimension_grid));
  ASSERT_ANY_THROW(make_polar_table<int>("b", "-", "", POEM_DOUBLE, dimension_grid));

  auto dimension_points = polar_table_double->dimension_points();

  // Filling the polar with values
  size_t idx = 0;
  for (const auto &dimension_point: polar_table_double->dimension_points()) {
    double val = 1.;
    for (const auto coord: dimension_point) {
      val *= coord;
    }

    polar_table_double->set_value(idx, val);
    polar_table_int->set_value(idx, idx);
    idx++;
  }

  // Interpolation
  DimensionPoint dimension_point(dimension_set, {1.2, 1.9, 2.8});

  auto val_interp = polar_table_double->interp(dimension_point, ERROR);
  double val_calc = 1.2 * 1.9 * 2.8;
  auto shape = polar_table_double->shape();

  auto val_interp_int = polar_table_int->interp(dimension_point, ERROR);
  auto val_nearest_int = polar_table_int->nearest(dimension_point, ERROR);
  // TODO: tester egalement le squeeze sur une table de int


  // Slicing
  std::unordered_map<std::string, double> slice{{"STW", 4}, // STW is out of bound
                                                {"TWS", 2.05}};

  ASSERT_ANY_THROW(polar_table_double->slice(slice, ERROR));
  ASSERT_NO_THROW(polar_table_double->slice(slice, SATURATE));

  slice["STW"] = 1.2; // STW is no more out of bound
  auto sliced_polar_table = polar_table_double->slice(slice, ERROR);

  // <Check that the sliced Polar is ok
  for (const auto &dimension_point_: sliced_polar_table->dimension_points()) {
    double val_calculated = 1.;
    for (const auto &coord: dimension_point_) {
      val_calculated *= coord;
    }
    double val_from_sliced_polar = sliced_polar_table->interp(dimension_point_, ERROR);

    ASSERT_EQ(val_from_sliced_polar, val_calculated);
  }

  auto sliced_shape = sliced_polar_table->shape();

  sliced_polar_table->squeeze();


  // Copy
  auto polar_table_copy = polar_table_double->copy();

  // Resampling
  auto new_dimension_grid = polar_table_double->dimension_grid()->copy();
  new_dimension_grid->set_values("STW", {1, 1.5, 2, 2.5, 3});
  auto resampled_polar_table = polar_table_double->resample(new_dimension_grid, ERROR);
  resampled_polar_table->change_name("VAR_resampled");


  for (const auto &dimension_point_: resampled_polar_table->dimension_points()) {
    double val_calculated = 1.;
    for (const auto &coord: dimension_point_) {
      val_calculated *= coord;
    }
    double val_from_polar = resampled_polar_table->interp(dimension_point_, ERROR);

    ASSERT_EQ(val_from_polar, val_calculated);
  }

  shape = resampled_polar_table->shape();

  // grid to index
  ASSERT_EQ(polar_table_double->dimension_grid()->grid_to_index({0, 1, 2}), 5);

  // Nearest
  dimension_point = {1.2, 2.1, 2.8};
  auto nearest_val = polar_table_double->nearest(dimension_point, ERROR);
  ASSERT_EQ(nearest_val, 6.);

  polar_table_double->sum(polar_table_double);

  // Writing
  to_netcdf(polar_table_double, "my_vessel", "polar_table.nc");

  // TODO: remettre
//  netCDF::NcFile dataFile_("polar_table.nc", netCDF::NcFile::read);
//
//  auto var = dataFile_.getVar("VAR");
//  std::shared_ptr<DimensionGrid> dimension_grid_read;
//  std::shared_ptr<PolarTableBase> polar_table_read;
//  read_polar_table(var, polar_table_read, dimension_grid_read, true);
//  dataFile_.close();
//
//  ASSERT_EQ(*polar_table, *polar_table_read->as_polar_table_double());

}


void fill(std::shared_ptr<PolarSet> polar_set) {

  // Dimensions
  auto STW_dim = make_dimension("STW_dim", "kt", "Speed Through Water");
  auto TWS_dim = make_dimension("TWS_dim", "kt", "True Wind Speed");
  auto TWA_dim = make_dimension("TWA_dim", "deg", "True Wind Angle");
  auto WA_dim = make_dimension("WA_dim", "deg", "Waves Angle");
  auto Hs_dim = make_dimension("Hs_dim", "m", "Waves Significant Height");
  auto Power_dim = make_dimension("Power_dim", "kW", "Brake Power");

  // DimensionSet
  auto dimension_set_speed_control = make_dimension_set({STW_dim, TWS_dim, TWA_dim, WA_dim, Hs_dim});
  auto dimension_set_power_control = make_dimension_set({Power_dim, TWS_dim, TWA_dim, WA_dim, Hs_dim});
  auto dimension_set_no_control = make_dimension_set({TWS_dim, TWA_dim, WA_dim, Hs_dim});

  // DimensionGrid
  auto STW_values = mathutils::linspace<double>(8, 20, 13);
  auto TWS_values = mathutils::linspace<double>(0, 40, 9);
  auto TWA_values = mathutils::linspace<double>(0, 180, 13);
  auto WA_values = mathutils::linspace<double>(0, 180, 13);
  auto Hs_values = mathutils::linspace<double>(0, 8, 9);
  auto Power_values = mathutils::linspace<double>(1000, 6500, 12);

  auto dimension_grid_speed_control = make_dimension_grid(dimension_set_speed_control);
  dimension_grid_speed_control->set_values("STW_dim", STW_values);
  dimension_grid_speed_control->set_values("TWS_dim", TWS_values);
  dimension_grid_speed_control->set_values("TWA_dim", TWA_values);
  dimension_grid_speed_control->set_values("WA_dim", WA_values);
  dimension_grid_speed_control->set_values("Hs_dim", Hs_values);

  auto dimension_grid_power_control = make_dimension_grid(dimension_set_power_control);
  dimension_grid_power_control->set_values("Power_dim", Power_values);
  dimension_grid_power_control->set_values("TWS_dim", TWS_values);
  dimension_grid_power_control->set_values("TWA_dim", TWA_values);
  dimension_grid_power_control->set_values("WA_dim", WA_values);
  dimension_grid_power_control->set_values("Hs_dim", Hs_values);

  auto dimension_grid_no_control = make_dimension_grid(dimension_set_no_control);
  dimension_grid_no_control->set_values("TWS_dim", TWS_values);
  dimension_grid_no_control->set_values("TWA_dim", TWA_values);
  dimension_grid_no_control->set_values("WA_dim", WA_values);
  dimension_grid_no_control->set_values("Hs_dim", Hs_values);

// Create some tables into the different Polar
  polar_set->create_polar(MPPP, dimension_grid_speed_control);
  polar_set->polar(MPPP)->create_polar_table<double>("TOTAL_POWER", "kW", "Total Power", POEM_DOUBLE)->fill_with(1000.);
  polar_set->polar(MPPP)->create_polar_table<double>("LEEWAY", "deg", "LEEWAY", POEM_DOUBLE)->fill_with(90.);
  polar_set->polar(MPPP)->create_polar_table<int>("SOLVER_STATUS", "-", "Solver Status", POEM_INT)->fill_with(1);

  polar_set->create_polar(HPPP, dimension_grid_speed_control);
  polar_set->polar(HPPP)->create_polar_table<double>("TOTAL_POWER", "kW", "Total Power", POEM_DOUBLE)->fill_with(1000.);
  polar_set->polar(HPPP)->create_polar_table<double>("LEEWAY", "deg", "LEEWAY", POEM_DOUBLE)->fill_with(90.);
  polar_set->polar(HPPP)->create_polar_table<int>("SOLVER_STATUS", "-", "Solver Status", POEM_INT)->fill_with(1);

  polar_set->create_polar(MVPP, dimension_grid_power_control);
  polar_set->polar(MVPP)->create_polar_table<double>("STW", "kt", "Speed Through Water", POEM_DOUBLE)->fill_with(10.);
  polar_set->polar(MVPP)->create_polar_table<double>("LEEWAY", "deg", "LEEWAY", POEM_DOUBLE)->fill_with(90.);
  polar_set->polar(MVPP)->create_polar_table<int>("SOLVER_STATUS", "-", "Solver Status", POEM_INT)->fill_with(1);

  polar_set->create_polar(HVPP, dimension_grid_power_control);
  polar_set->polar(HVPP)->create_polar_table<double>("STW", "kt", "Speed Through Water", POEM_DOUBLE)->fill_with(10.);
  polar_set->polar(HVPP)->create_polar_table<double>("LEEWAY", "deg", "LEEWAY", POEM_DOUBLE)->fill_with(90.);
  polar_set->polar(HVPP)->create_polar_table<int>("SOLVER_STATUS", "-", "Solver Status", POEM_INT)->fill_with(1);

  polar_set->create_polar(VPP, dimension_grid_no_control);
  polar_set->polar(VPP)->create_polar_table<double>("STW", "kt", "Speed Through Water", POEM_DOUBLE)->fill_with(10.);
  polar_set->polar(VPP)->create_polar_table<double>("LEEWAY", "deg", "LEEWAY", POEM_DOUBLE)->fill_with(90.);
  polar_set->polar(VPP)->create_polar_table<int>("SOLVER_STATUS", "-", "Solver Status", POEM_INT)->fill_with(1);

}


TEST(poem, PolarNode) {

  auto vessel = make_polar_node("vessel", "my vessel");
  ASSERT_TRUE(vessel->is_root());


  auto ballast_load = make_polar_node("ballast_load", "Ballast load case");
  vessel->add_child(ballast_load);

  auto ballast_one_engine = make_polar_set("ballast_one_engine",
                                           "Ballast load case with one engine");
  ballast_load->add_child(ballast_one_engine);

  auto ballast_two_engines = make_polar_set("ballast_two_engines",
                                            "Ballast load case with two engines");
  ballast_load->add_child(ballast_two_engines);

  auto laden_load = make_polar_node("laden_load", "Laden load case");
  vessel->add_child(laden_load);

  auto laden_one_engine = make_polar_set("laden_one_engine",
                                         "Laden load case with one engine");
  laden_load->add_child(laden_one_engine);

  auto laden_two_engines = make_polar_set("laden_two_engines",
                                          "Laden load case with two engines");
  laden_load->add_child(laden_two_engines);

  /*
   * vessel_name ->ballast_load -> ballast_one_engine
   *                            -> ballast_two_engines
   *             ->laden_load   -> laden_one_engine
   *                            ->laden_two_engines
   */

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
  ASSERT_EQ(ballast_one_engine->polar(MPPP)->polar_table("TOTAL_POWER")->full_name(),
            "/vessel/ballast_load/ballast_one_engine/MPPP/TOTAL_POWER");

  // Writing
  to_netcdf(vessel, "vessel", "poem_testing_spec_v1.nc");
  // Reading back
  auto vessel_ = load("poem_testing_spec_v1.nc");
  // Writing once again
  to_netcdf(vessel_, "vessel", "poem_testing_spec_v1_.nc");

  // Reading once again
  auto vessel_2 = load("poem_testing_spec_v1_.nc");

  ASSERT_EQ(*vessel_, *vessel_2);

}

TEST(poem, read_poem_v0_example) {

  ASSERT_ANY_THROW(load("dont_exist.nc"));

  auto vessel = load("poem_v0_example_no_sails.nc", "vessel");

  // Generating the layout
  auto layout = vessel->layout();
  std::ofstream output("layout.json");
  output << std::setw(2) << layout << std::endl;


  std::vector<std::string> polar_tables_paths;
  vessel->polar_tables_paths(polar_tables_paths);

  for (const auto &path: polar_tables_paths) {
    auto polar_table = vessel->polar_node_from_path(path)->as_polar_table();
    switch (polar_table->type()) {
      case poem::POEM_DOUBLE: {
        auto polar_table_double = polar_table->as_polar_table_double();
        break;
      }

      case poem::POEM_INT: {
        auto polar_table_int = polar_table->as_polar_table_int();
        break;
      }
    }
  }


//  auto json = vessel->layout();
//  std::cout << json.dump(2) << std::endl;

  to_netcdf(vessel, "my_vessel", "poem_v0_example_no_sails_v0_to_last_version.nc");

  int version = get_version("poem_v0_example_no_sails_v0_to_last_version.nc");
  if (spec_check("poem_v0_example_no_sails_v0_to_last_version.nc", version)) {
    LogNormalInfo("File is compliant with version v{}", version);
  }

}


//TEST(poem, mount) {
//
//
//  auto from_root_node = load("MPPP.nc", true, false);
//  auto to_root_node = load("HPPP.nc", true, false);
//
//
//}


//// TODO: SUPPRIMER !!!
//TEST(poem, other) {
//
////  auto vessel = load("/home/frongere/Documents/DEV_SOFTWARE/poem/tmp/nok/poem_testing_spec_v1.nc");
//  auto vessel =
//      load("/home/frongere/Documents/DEV_SOFTWARE/poem/tmp/b8b20d7d7e90d74ecec3f42cb9dbfa3d37aa26c5925dc9d2bb3c9e4912c6382f.nc");
//
//  auto layout = vessel->layout();
////  std::ofstream output("layout.json");
////  output << std::setw(2) << layout << std::endl;
//  std::cout << layout.dump(2) << std::endl;
//
//}
