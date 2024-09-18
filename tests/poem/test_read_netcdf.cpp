//
// Created by frongere on 13/04/23.
//

#include <string>
#include <filesystem>

#include <gtest/gtest.h>
#include "poem/poem.h"

using namespace poem;

namespace fs = std::filesystem;

TEST(poem_, reader) {

  // PolarSetReader reader;

  std::string polar_file = fs::path(POEM_RESOURCE_DIR) / "poem_v1_PolarSet_PPP.nc";

  // smichel this class has to be debug
  // PolarSetReader psr(polar_file);
  // std::shared_ptr<PolarSet> ps_ptr = psr.polar_set();
  // auto attributes = psr.attributes();

  // std::cout << "- PolarSet name: " << ps_ptr->name() << std::endl;
  // std::cout << "  PolarSet type: " << ps_ptr->polar_type() << " or " << ps_ptr->polar_type_str() << std::endl;
  // std::cout << "  PolarSet contains: ";
  // auto polar_names = ps_ptr->polar_names();
  // for (auto p : polar_names){
  //   std::cout << p <<", ";
  // } 
  // std::cout << std::endl;
  // std::string pn = ps_ptr->polar_type_str();
  // std::array<double, 5> dimension_point{10.1, 10.1, 90.1, 0, 0};
  // std::cout << "  Dimension point: (";
  // for (auto i : dimension_point)
  // {
  //   std::cout << i << ",";
  // }
  // std::cout << ")" << std::endl;
  // for (auto p : polar_names){
  //   std::shared_ptr<InterpolablePolar<5>> polar = ps_ptr->polar<double, 5>(p);
  //   std::cout << "  Creation of the InterpolableTable (" << p << ") - DONE " << std::endl;
  //   const int dim = polar->dim();
  //   std::cout << "  Polar type: " << polar->polar_type() << std::endl;
  //   std::cout << "  Polar dim: " << polar->dim() << std::endl;     
  //   std::cout << "  Polar size: " << polar->size() << std::endl;     
  //   std::cout << "  Polar is filled ?: " << polar->is_filled() << std::endl;     
  //   auto dim_name = polar->get_dimension_set()->get_names();
  //   for (auto n: dim_name){
  //     std::cout << "    Dimension " << n;
  //     std::cout << " - min: " << polar->min_bounds(n);
  //     std::cout << " - max: " << polar->max_bounds(n) << std::endl;
  //   }
    
  //   // auto dpl = polar->dimension_point_list();
  //   // for (auto dp: dim_set){
  //   //   std::cout << dp << std::endl;
  //   // }

  //   auto nr = polar->nearest(dimension_point, true);
  //   std::cout << "  Nearest: " << nr << std::endl;
  //   auto interp = polar->interp(dimension_point, true);
  //   std::cout << "  Interpolation: " << interp << std::endl;
  //   std::string new_polar_file = fs::path(POEM_RESOURCE_DIR) / "my_favorite_InterpolableTable_";
  //   new_polar_file.append(ps_ptr->polar_type_str());
  //   new_polar_file.append("_");
  //   new_polar_file.append(p);
  //   new_polar_file.append(".nc");
  //   std::cout << "I like this InterpolableTable, I save it in " << new_polar_file << std::endl;
  //   // ps_ptr->to_netcdf(new_polar_file);

  // }  
  // std::string new_polar_file = fs::path(POEM_RESOURCE_DIR) / "my_favorite_PolarSet_";
  // new_polar_file.append(ps_ptr->polar_type_str());
  // new_polar_file.append(".nc");
  // std::cout << "I like this PolarSet, I save it in " << new_polar_file << std::endl;
  // // ps_ptr->to_netcdf(new_polar_file);
}
