# import poem
import os
import logging
# import numpy as np

import pypoem
# logging.basicConfig(level=logging.info)

""" FOR TOMORROW => MAKE THE FIX FOR SHARED_PTR OBJECT , shall be direct with *"""
def test_poem():
    pass
# "src/pypoem/poem.cpython-310-x86_64-linux-gnu.so"
POEM_RESOURCE_DIR= '/home/smichel/softwares/poem_gitlab/build/resources'
polar_file = os.path.join(POEM_RESOURCE_DIR, "poem_v1_example.nc")

PerfPolarSet = pypoem.read_performance_polar_set(polar_file)

print("====================== LET'S START test_read_poem.cpp")
print("Hola, this unit test load and read everything it can from {}:".format(polar_file))

print("Attribute name: {}".format(PerfPolarSet.name()))

#   // smichel: pb, need to be fixed
#   std::vector<POLAR_TYPE> all_polar_type_list{PPP, HPPP, HVPP, MVPP, VPP};
#   for (auto pl : all_polar_type_list)
#   {
#     auto name = polar_type_enum2s(pl);
#     std::cout << "Does my PerfPolarSet contains " << name << " ? => ";
#     std::cout << pps_ptr->exist(pl) << std::endl;
#   }

polar_set_list = PerfPolarSet.polar_set_list()
print( "Polar set list (names): {}".format(polar_set_list))

print("\n\n======================")
print("For each PolarSet, return info ")
print("======================")

print("Construction of PolarSet from the polar_set function")
for p in polar_set_list:
    print("Setting of {} as a PolarSet".format(p))
    PolarSet = PerfPolarSet.get_polar_set(p)
    print("Setting of {} as a PolarSet - DONE".format(p))

    print("- PolarSet name: {}".format(PolarSet.name()))
    print("  PolarSet type: {}".format(PolarSet.polar_type_str()))
    polar_names=PolarSet.polar_names()
    print("  PolarSet contains: {}".format(polar_names))
    dimension_point = [10.1, 10.1, 90.1, 0, 0]
    print("Dimension point: {}".format(dimension_point))

    
#     for (auto p : polar_names){
#       std::shared_ptr<InterpolablePolar<5>> polar = ps_ptr->polar<double, 5>(p);
#       std::cout << "  Creation of the InterpolableTable (" << p << ") - DONE " << std::endl;
#       const int dim = polar->dim();
#       std::cout << "  Polar type: " << polar->polar_type() << std::endl;
#       std::cout << "  Polar dim: " << polar->dim() << std::endl;     
#       std::cout << "  Polar size: " << polar->size() << std::endl;     
#       std::cout << "  Polar is filled ?: " << polar->is_filled() << std::endl;     
#       auto dim_name = polar->get_dimension_set_names();
#       for (auto n: dim_name){
#         std::cout << "    Dimension " << n;
#         std::cout << " - min: " << polar->min_bounds(n);
#         std::cout << " - max: " << polar->max_bounds(n) << std::endl;
#       }
      
#       // auto dpl = polar->dimension_point_list();
#       // for (auto dp: dim_set){
#       //   std::cout << dp << std::endl;
#       // }

#       auto nr = polar->nearest(dimension_point, true);
#       std::cout << "  Nearest: " << nr << std::endl;
#       auto interp = polar->interp(dimension_point, true);
#       std::cout << "  Interpolation: " << interp << std::endl;
#       std::string new_polar_file = fs::path(POEM_RESOURCE_DIR) / "my_favorite_InterpolableTable_";
#       new_polar_file.append(ps_ptr->polar_type_str());
#       new_polar_file.append("_");
#       new_polar_file.append(p);
#       new_polar_file.append(".nc");
#       std::cout << "I like this InterpolableTable, I save it in " << new_polar_file << std::endl;
#       // ps_ptr->to_netcdf(new_polar_file);

#     }  
#     std::string new_polar_file = fs::path(POEM_RESOURCE_DIR) / "my_favorite_PolarSet_";
#     new_polar_file.append(ps_ptr->polar_type_str());
#     new_polar_file.append(".nc");
#     std::cout << "I like this PolarSet, I save it in " << new_polar_file << std::endl;
#     // ps_ptr->to_netcdf(new_polar_file);
#   }

#   std::cout << std::endl
#             << "======================" << std::endl;

#   std::cout << "Construction of PolarSet from the iteraton" << std::endl;
#   // construct polars
#   for (auto mps = pps_ptr->begin(); mps != pps_ptr->end(); mps++)
#   {
#     // std::shared_ptr<PolarSet> ps_ptr = pps_ptr->polar_set(polar_name);
#     auto ps_ptr = (*mps).second;
#     std::cout << "- PolarSet name: " << ps_ptr->name() << std::endl;
#     std::cout << "  PolarSet type: " << ps_ptr->polar_type() << " or " << ps_ptr->polar_type_str() << std::endl;
#     std::string pn = ps_ptr->polar_type_str();
#     std::array<double, 5> dimension_point{10.1, 10.1, 90.1, 0, 0};
#     std::cout << "  Dimension point: (";
#     for (auto i : dimension_point)
#     {
#       std::cout << i << ",";
#     }
#     std::cout << ")" << std::endl;
#   }
#   std::string new_polar_file = fs::path(POEM_RESOURCE_DIR) / "my_favorite_PerfPolarSet.nc";
#   std::cout << "I like this PerformancePolarSet, I save it in " << new_polar_file << std::endl;
#   // pps_ptr->to_netcdf(new_polar_file);