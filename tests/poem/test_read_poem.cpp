//
// Created by frongere on 13/04/23.
//

#include <string>
#include <filesystem>

#include <gtest/gtest.h>
#include "poem/poem.h"

using namespace poem;

namespace fs = std::filesystem;

TEST(poem_, readerInterp) {

  std::string polar_file = fs::path(POEM_RESOURCE_DIR) / "poem_v1_PerfPolarSet.nc";

  std::shared_ptr<PerformancePolarSet> pps_ptr = read_performance_polar_set(polar_file);

  std::cout << std::endl
            << "====================== LET'S START test_read_poem.cpp" << std::endl;
  std::cout << "Hola, this unit test load " << polar_file << " and read everything it can :)" << std::endl;

  // PerformancePolarSet perf_polar_set = pps_ptr.get();
  auto res0 = pps_ptr->name();
  std::cout << "Attribute name: " << res0 << std::endl;

  // smichel: pb, need to be fixed
  std::vector<POLAR_TYPE> all_polar_type_list{PPP, HPPP, HVPP, MVPP, VPP};
  for (auto pl: all_polar_type_list) {
    auto name = polar_type_enum2s(pl);
    std::cout << "Does my PerfPolarSet contains " << name << " ? => ";
    std::cout << pps_ptr->exist(pl) << std::endl;
  }

  std::vector<POLAR_TYPE> res = pps_ptr->polar_set_type_list();
  std::cout << "Polar set list (names): ";
  for (auto r: res) {
    std::cout << polar_type_enum2s(r) << ", ";
  }

  std::cout << std::endl
            << std::endl
            << "======================" << std::endl;
  std::cout << "For each PolarSet, return info " << std::endl;
  std::cout << "======================" << std::endl;

  std::cout << "Construction of PolarSet from the polar_set function" << std::endl;
  // construct polars
  for (auto polar_name: res) {
    std::shared_ptr<PolarSet> ps_ptr = pps_ptr->polar_set(polar_name);

    std::cout << "- PolarSet name: " << ps_ptr->name() << std::endl;
    std::cout << "  PolarSet type: " << ps_ptr->polar_type() << " or " << ps_ptr->polar_type_str() << std::endl;
    std::cout << "  PolarSet contains: ";
    auto polar_names = ps_ptr->polar_names();
    for (auto p: polar_names) {
      std::cout << p << ", ";
    }
    std::cout << std::endl;
    std::string pn = ps_ptr->polar_type_str();
    std::array<double, 5> dimension_point{10.1, 10.1, 90.1, 0, 0};
    std::cout << "  Dimension point: (";
    for (auto i: dimension_point) {
      std::cout << i << ",";
    }
    std::cout << ")" << std::endl;
    for (auto p: polar_names) {
      std::shared_ptr<InterpolablePolar<5> > polar = ps_ptr->polar<double, 5>(p);
      std::cout << "  Creation of the InterpolableTable (" << p << ") - DONE " << std::endl;
      const int dim = polar->dim();
      std::cout << "  Polar type: " << polar->polar_type() << std::endl;
      std::cout << "  Polar dim: " << polar->dim() << std::endl;
      std::cout << "  Polar size: " << polar->size() << std::endl;
      std::cout << "  Polar is filled ?: " << polar->is_filled() << std::endl;
      auto dim_name = polar->dimension_set_names();
      for (auto n: dim_name) {
        std::cout << "    Dimension " << n;
        auto coord = polar->coordinates(n);
        std::cout << "  List: (";
        for (auto i: coord) {
          std::cout << i << ",";
        }
        std::cout << ")" << std::endl;
        std::cout << " - min: " << polar->min_bounds(n);
        std::cout << " - max: " << polar->max_bounds(n) << std::endl;
      }
      std::cout << "    Min & max values of " << p;
      std::cout << " - min: " << polar->min_value();
      std::cout << " - max: " << polar->max_value() << std::endl;

      // auto dpl = polar->dimension_point_list();
      // for (auto dp: dim_set){
      //   std::cout << dp << std::endl;
      // }

      auto nr = polar->nearest(dimension_point, true);
      std::cout << "  Nearest: " << nr << std::endl;
      auto interp = polar->interp(dimension_point, true);
      std::cout << "  Interpolation: " << interp << std::endl;
      // std::string new_polar_file = fs::path(POEM_RESOURCE_DIR) / "my_favorite_InterpolableTable_";
      // new_polar_file.append(ps_ptr->polar_type_str());
      // new_polar_file.append("_");
      // new_polar_file.append(p);
      // new_polar_file.append(".nc");
      // std::cout << "I like this InterpolableTable, I save it in " << new_polar_file << std::endl;
      // ps_ptr->to_netcdf(new_polar_file);
    }
    // std::string new_polar_file = fs::path(POEM_RESOURCE_DIR) / "my_favorite_PolarSet_";
    // new_polar_file.append(ps_ptr->polar_type_str());
    // new_polar_file.append(".nc");
    // std::cout << "I like this PolarSet, I save it in " << new_polar_file << std::endl;
    // ps_ptr->to_netcdf(new_polar_file);
  }

  std::cout << std::endl
            << "======================" << std::endl;

  std::cout << "Construction of PolarSet from the iteraton" << std::endl;
  // construct polars
  for (auto mps = pps_ptr->begin(); mps != pps_ptr->end(); mps++) {
    // std::shared_ptr<PolarSet> ps_ptr = pps_ptr->polar_set(polar_name);
    auto ps_ptr = (*mps).second;
    std::cout << "- PolarSet name: " << ps_ptr->name() << std::endl;
    std::cout << "  PolarSet type: " << ps_ptr->polar_type() << " or " << ps_ptr->polar_type_str() << std::endl;
    std::string pn = ps_ptr->polar_type_str();
    std::array<double, 5> dimension_point{10.1, 10.1, 90.1, 0, 0};
    std::cout << "  Dimension point: (";
    for (auto i: dimension_point) {
      std::cout << i << ",";
    }
    std::cout << ")" << std::endl;
  }
  std::string new_polar_file = fs::path(POEM_RESOURCE_DIR) / "my_favorite_PerfPolarSet.nc";
  std::cout << "I like this PerformancePolarSet, I save it in " << new_polar_file << std::endl;
  // pps_ptr->to_netcdf(new_polar_file);
}

TEST(poemfull_, reader) {

  std::string polar_file = fs::path(POEM_RESOURCE_DIR) / "poem_v1_example_5_polars.nc";

  std::shared_ptr<PerformancePolarSet> pps_ptr = read_performance_polar_set(polar_file);

  std::cout << std::endl
            << "====================== LET'S START test_read_poem.cpp" << std::endl;
  std::cout << "Hola, this unit test load " << polar_file << " and read everything it can :)" << std::endl;

  // PerformancePolarSet perf_polar_set = pps_ptr.get();
  auto res0 = pps_ptr->name();
  std::cout << "Attribute name: " << res0 << std::endl;

  // smichel: pb, need to be fixed
  std::vector<POLAR_TYPE> all_polar_type_list{PPP, HPPP, HVPP, MVPP, VPP};
  for (auto pl: all_polar_type_list) {
    auto name = polar_type_enum2s(pl);
    std::cout << "Does my PerfPolarSet contains " << name << " ? => ";
    std::cout << pps_ptr->exist(pl) << std::endl;
  }

  std::vector<POLAR_TYPE> res = pps_ptr->polar_set_type_list();
  std::cout << "Polar set list (names): ";
  for (auto r: res) {
    std::cout << r << ", ";
  }

  std::cout << std::endl
            << std::endl
            << "======================" << std::endl;
  std::cout << "For each PolarSet, return info " << std::endl;
  std::cout << "======================" << std::endl;

  std::cout << "Construction of PolarSet from the polar_set function" << std::endl;
  // construct polars
  for (auto polar_name: res) {
    std::shared_ptr<PolarSet> ps_ptr = pps_ptr->polar_set(polar_name);

    std::cout << "- PolarSet name: " << ps_ptr->name() << std::endl;
    std::cout << "  PolarSet type: " << ps_ptr->polar_type() << " or " << ps_ptr->polar_type_str() << std::endl;
    std::cout << "  PolarSet contains: ";
    auto polar_names = ps_ptr->polar_names();
    for (auto p: polar_names) {
      std::cout << p << ", ";
    }
    std::cout << std::endl;
    std::string pn = ps_ptr->polar_type_str();
    std::array<double, 5> dimension_point{10.1, 10.1, 90.1, 0, 0};
  //   std::cout << "  Dimension point: (";
  //   for (auto i : dimension_point)
  //   {
  //     std::cout << i << ",";
  //   }
  //   std::cout << ")" << std::endl;
  //   for (auto p : polar_names)
  //   {
  //     std::shared_ptr<InterpolablePolar<5> > polar = ps_ptr->polar<double, 5>(p);
  //     std::cout << "  Creation of the InterpolableTable (" << p << ") - DONE " << std::endl;
  //     const int dim = polar->dim();
  //     std::cout << "  Polar type: " << polar->polar_type() << std::endl;
  //     std::cout << "  Polar dim: " << polar->dim() << std::endl;
  //     std::cout << "  Polar size: " << polar->size() << std::endl;
  //     std::cout << "  Polar is filled ?: " << polar->is_filled() << std::endl;
  //     auto dim_name = polar->dimension_set_names();
  //     for (auto n : dim_name)
  //     {
  //       std::cout << "    Dimension " << n;
  //       std::cout << " - min: " << polar->min_bounds(n);
  //       std::cout << " - max: " << polar->max_bounds(n) << std::endl;
  //     }
  //     std::cout << "    Min & max values of " << p;
  //     std::cout << " - min: " << polar->min_value();
  //     std::cout << " - max: " << polar->max_value() << std::endl;
  }

  std::cout << std::endl
            << "======================" << std::endl;

}

TEST(poem_, reader_writer) {

  for (int iversion = POEM_MIN_READ_FILE_VERSION; iversion < POEM_MAX_FILE_VERSION+1; ++iversion) {

    std::string filename = std::string("poem_v") + std::to_string(iversion) + "_example.nc";
    std::string polar_file = fs::path(POEM_RESOURCE_DIR) / filename;

    // version of the polar
    spdlog::info("Reading file version v{}", iversion);
    auto reader = Reader(polar_file);

    // read the polar and store it into a PerformancePolarSet
    std::shared_ptr<PerformancePolarSet> perf_polar_set = reader.read();

    // write it with different version of the file
    spdlog::info("Writing to latest polar file version v{}", POEM_MAX_FILE_VERSION);

    perf_polar_set->to_netcdf("new.nc", true);

    // check that the files are compliant with last version
    auto last_spec_rules = SpecRules(POEM_MAX_FILE_VERSION);
    ASSERT_TRUE(last_spec_rules.check("new.nc", true));

  }

}


//TEST(poem_, reader_write_v0) {
//
//  std::string polar_file = fs::path(POEM_RESOURCE_DIR) / "poem_v0_example.nc";
//
//  // version of the polar
//  spdlog::info("Reading file version v0");
//  auto reader = Reader(polar_file);
//
//  // read the polar and store it into a PerformancePolarSet
//  std::shared_ptr<PerformancePolarSet> perf_polar_set = reader.read();
//
//  // write it with different version of the file
//  spdlog::info("Writing to latest polar file version v{}", POEM_MAX_FILE_VERSION);
//  perf_polar_set->to_netcdf("new.nc", true);
//
//  // check that the files are compliant with last version
//  auto spec_rules = SpecRules(POEM_MAX_FILE_VERSION);
//  ASSERT_TRUE(spec_rules.check("new.nc", true));
//
//}
