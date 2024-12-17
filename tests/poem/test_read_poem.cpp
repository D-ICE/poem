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

  std::shared_ptr<PolarSet> polar_set = read_polar_set(polar_file);

  std::cout << std::endl
            << "====================== LET'S START test_read_poem.cpp" << std::endl;
  std::cout << "Hola, this unit test load " << polar_file << " and read everything it can :)" << std::endl;

  // PolarSet perf_polar_set = polar_set.get();
  auto polar_set_name = polar_set->name();
  std::cout << "Attribute name: " << polar_set_name << std::endl;

  // smichel: pb, need to be fixed
  std::vector<POLAR_TYPE> all_polar_type_list{PPP, HPPP, HVPP, MVPP, VPP};
  for (auto polar_type: all_polar_type_list) {
    auto name = polar_type_enum2s(polar_type);
    std::cout << "Does my PerfPolarSet contains " << name << " ? => ";
    std::cout << polar_set->exist(polar_type) << std::endl;
  }

  std::vector<POLAR_TYPE> polar_type_list = polar_set->polar_type_list();
  std::cout << "Polar set list (names): ";
  for (auto r: polar_type_list) {
    std::cout << polar_type_enum2s(r) << ", ";
  }

  std::cout << std::endl
            << std::endl
            << "======================" << std::endl;
  std::cout << "For each Polar, return info " << std::endl;
  std::cout << "======================" << std::endl;

  std::cout << "Construction of Polar from the polar function" << std::endl;
  // construct polars
  for (auto polar_name: polar_type_list) {
    std::shared_ptr<Polar> polar = polar_set->polar(polar_name);

    std::cout << "- Polar name: " << polar->name() << std::endl;
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
      std::shared_ptr<InterpolablePolarTable<5> > polar_table = polar->polar_table<double, 5>(p);
      std::cout << "  Creation of the InterpolableTable (" << p << ") - DONE " << std::endl;
      const int dim = polar_table->dim();
      std::cout << "  Polar type: " << polar_table->polar_type() << std::endl;
      std::cout << "  Polar dim: " << polar_table->dim() << std::endl;
      std::cout << "  Polar size: " << polar_table->size() << std::endl;
      std::cout << "  Polar is filled ?: " << polar_table->is_filled() << std::endl;
      auto dim_name = polar_table->dimension_set_names();
      for (auto n: dim_name) {
        std::cout << "    Dimension " << n;
//        auto coord = polar_table->coordinates(n);
//        std::cout << "  List: (";
//        for (auto i: coord) {
//          std::cout << i << ",";
//        }
        std::cout << ")" << std::endl;
        std::cout << " - min: " << polar_table->min_bound(n);
        std::cout << " - max: " << polar_table->max_bound(n) << std::endl;
      }
      std::cout << "    Min & max values of " << p;
      std::cout << " - min: " << polar_table->min_value();
      std::cout << " - max: " << polar_table->max_value() << std::endl;

      // auto dpl = polar_table->dimension_point_list();
      // for (auto dp: dim_set){
      //   std::cout << dp << std::endl;
      // }

      auto nr = polar_table->nearest(dimension_point, true);
      std::cout << "  Nearest: " << nr << std::endl;
      auto interp = polar_table->interp(dimension_point, true);
      std::cout << "  Interpolation: " << interp << std::endl;
      // std::string new_polar_file = fs::path(POEM_RESOURCE_DIR) / "my_favorite_InterpolableTable_";
      // new_polar_file.append(polar->polar_type_str());
      // new_polar_file.append("_");
      // new_polar_file.append(p);
      // new_polar_file.append(".nc");
      // std::cout << "I like this InterpolableTable, I save it in " << new_polar_file << std::endl;
      // polar->to_netcdf(new_polar_file);
    }
    // std::string new_polar_file = fs::path(POEM_RESOURCE_DIR) / "my_favorite_PolarSet_";
    // new_polar_file.append(polar->polar_type_str());
    // new_polar_file.append(".nc");
    // std::cout << "I like this Polar, I save it in " << new_polar_file << std::endl;
    // polar->to_netcdf(new_polar_file);
  }

  std::cout << std::endl
            << "======================" << std::endl;

  std::cout << "Construction of Polar from the iteraton" << std::endl;
  // construct polars
  for (auto mps = polar_set->begin(); mps != polar_set->end(); mps++) {
    // std::shared_ptr<Polar> polar_ = polar_set->polar(polar_name);
    auto polar_ = (*mps).second;
    std::cout << "- Polar name: " << polar_->name() << std::endl;
    std::cout << "  Polar type: " << polar_->polar_type() << " or " << polar_->polar_type_str() << std::endl;
    std::string pn = polar_->polar_type_str();
    std::array<double, 5> dimension_point{10.1, 10.1, 90.1, 0, 0};
    std::cout << "  Dimension point: (";
    for (auto i: dimension_point) {
      std::cout << i << ",";
    }
    std::cout << ")" << std::endl;
  }
  std::string new_polar_file = fs::path(POEM_RESOURCE_DIR) / "my_favorite_PolarSet.nc";
  std::cout << "I like this PolarSet, I save it in " << new_polar_file << std::endl;
  // polar_set->to_netcdf(new_polar_file);
}

TEST(poemfull_, reader) {

  std::string polar_file = fs::path(POEM_RESOURCE_DIR) / "poem_v1_example_5_polars.nc";

  std::shared_ptr<PolarSet> polar_set = read_polar_set(polar_file);

  std::cout << std::endl
            << "====================== LET'S START test_read_poem.cpp" << std::endl;
  std::cout << "Hola, this unit test load " << polar_file << " and read everything it can :)" << std::endl;

  // PolarSet perf_polar_set = polar_set.get();
  auto polar_set_name = polar_set->name();
  std::cout << "Attribute name: " << polar_set_name << std::endl;

  // smichel: pb, need to be fixed
  std::vector<POLAR_TYPE> all_polar_type_list{PPP, HPPP, HVPP, MVPP, VPP};
  for (auto polar_type: all_polar_type_list) {
    auto name = polar_type_enum2s(polar_type);
    std::cout << "Does my PerfPolarSet contains " << name << " ? => ";
    std::cout << polar_set->exist(polar_type) << std::endl;
  }

  std::vector<POLAR_TYPE> polar_type_list = polar_set->polar_type_list();
  std::cout << "Polar set list (names): ";
  for (auto r: polar_type_list) {
    std::cout << r << ", ";
  }

  std::cout << std::endl
            << std::endl
            << "======================" << std::endl;
  std::cout << "For each Polar, return info " << std::endl;
  std::cout << "======================" << std::endl;

  std::cout << "Construction of Polar from the polar function" << std::endl;
  // construct polars
  for (auto polar_type: polar_type_list) {
    std::shared_ptr<Polar> polar_ = polar_set->polar(polar_type);

    std::cout << "- Polar name: " << polar_->name() << std::endl;
    std::cout << "  Polar type: " << polar_->polar_type() << " or " << polar_->polar_type_str() << std::endl;
    std::cout << "  Polar contains: ";
    auto polar_names = polar_->polar_names();
    for (auto polar_name: polar_names) {
      std::cout << polar_name << ", ";
    }
    std::cout << std::endl;
    std::string pn = polar_->polar_type_str();
    std::array<double, 5> dimension_point{10.1, 10.1, 90.1, 0, 0};
  //   std::cout << "  Dimension point: (";
  //   for (auto i : dimension_point)
  //   {
  //     std::cout << i << ",";
  //   }
  //   std::cout << ")" << std::endl;
  //   for (auto p : polar_names)
  //   {
  //     std::shared_ptr<InterpolablePolarTable<5> > polar = polar_->polar<double, 5>(p);
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
  //       std::cout << " - min: " << polar->min_bound(n);
  //       std::cout << " - max: " << polar->max_bound(n) << std::endl;
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

    // read the polar and store it into a PolarSet
    std::shared_ptr<PolarSet> polar_set = reader.read();

    // write it with different version of the file
    spdlog::info("Writing to latest polar file version v{}", POEM_MAX_FILE_VERSION);

    polar_set->to_netcdf("new.nc", true);

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
//  // read the polar and store it into a PolarSet
//  std::shared_ptr<PolarSet> perf_polar_set = reader.read();
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
