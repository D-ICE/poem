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
polar_file = os.path.join(POEM_RESOURCE_DIR, "poem_v1_example_5_polars.nc")
# polar_file = os.path.join(POEM_RESOURCE_DIR, "poem_v1_example.nc")

PerfPolarSet = pypoem.read_performance_polar_set(polar_file)

print("====================== LET'S START test_read_poem.cpp")
print("Hola, this unit test load and read everything it can from {}:".format(polar_file))

print("Attribute name: {}".format(PerfPolarSet.name()))

#   // smichel: pb, need to be fixed
#   all_polar_type_list = {PPP, HPPP, HVPP, MVPP, VPP};
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
    PolarSet = PerfPolarSet.polar_set(p)
    print("Setting of {} as a PolarSet - DONE".format(p))

    print("- PolarSet name: {}".format(PolarSet.name()))
    print("  PolarSet type: {}".format(PolarSet.polar_type_str()))
    polar_names=PolarSet.polar_names()
    print("  PolarSet contains: {}".format(polar_names))
    if PolarSet.polar_type_str() == 'VPP':
        dimension_point = [10.1, 90.1, 0, 0]
        print("  Dimension point: {}".format(dimension_point))
        #
        for pln in polar_names:
            print("\n  Creation of the InterpolableTable ({}) ".format(pln))
            my4dpolar = PolarSet.get_4D_polar(pln)
            print("  Creation of the InterpolableTable ({}) - DONE ".format(pln))
            print("  Polar dim: {}".format(my4dpolar.dim()))
            # print("  Polar type: {}".format(my5dpolar.polar_type())) // polar type object needed > but useless, the info is already in the PolarSet
            print("  Polar size: {}".format(my4dpolar.size()))
            print("  Polar is filled: {}".format(my4dpolar.is_filled()))
            dim_names = my4dpolar.dimension_set_names()
            for dn in dim_names:
                print("  Dimension {}: min={}, max={}".format(dn, my4dpolar.min_bounds(dn), my4dpolar.max_bounds(dn)))
                print("  {}".format(my4dpolar.coordinates(dn)))
        
            print("    Min & max values of {}: min={}, max={}".format(p,my4dpolar.min_value(),my4dpolar.max_value()))
            nr = my4dpolar.nearest(dimension_point,True)
            print("  Nearest: {}".format(nr))
            interp = my4dpolar.interp(dimension_point,True)
            print("  Interpolation: {}".format(interp))
    
    else:
        if PolarSet.polar_type_str() == 'HVPP' or PolarSet.polar_type_str() == 'MVPP':
            dimension_point = [3000, 10.1, 90.1, 0, 0]
        else:
            dimension_point = [10.1, 10.1, 90.1, 0, 0]
        print("  Dimension point: {}".format(dimension_point))
        #
        for pln in polar_names:
            print("\n  Creation of the InterpolableTable ({}) ".format(pln))
            my5dpolar = PolarSet.get_5D_polar(pln)
            print("  Creation of the InterpolableTable ({}) - DONE ".format(pln))
            print("  Polar dim: {}".format(my5dpolar.dim()))
            # print("  Polar type: {}".format(my5dpolar.polar_type())) // polar type object needed > but useless, the info is already in the PolarSet
            print("  Polar size: {}".format(my5dpolar.size()))
            print("  Polar is filled: {}".format(my5dpolar.is_filled()))
            dim_names = my5dpolar.dimension_set_names()
            for dn in dim_names:
                print("  Dimension {}: min={}, max={}".format(dn, my5dpolar.min_bounds(dn), my5dpolar.max_bounds(dn)))
                print("  {}".format(my5dpolar.coordinates(dn)))
        
            print("    Min & max values of {}: min={}, max={}".format(p,my5dpolar.min_value(),my5dpolar.max_value()))
    #       // auto dpl = polar->dimension_point_list();
    #       // for (auto dp: dim_set){
    #       //   std::cout << dp << std::endl;
    #       // }
            nr = my5dpolar.nearest(dimension_point,True)
            print("  Nearest: {}".format(nr))
            interp = my5dpolar.interp(dimension_point,True)
            print("  Interpolation: {}".format(interp))

print("\n====================== END OF THE TEST")
