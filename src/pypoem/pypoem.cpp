// #include <pybind11/chrono.h>
// #include <pybind11/eigen.h>
// #include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <string>
#include "poem/poem.h"

namespace py = pybind11;

PYBIND11_MODULE(pypoem, m) {
  using namespace poem;

  // m.doc() = "pybind11 example plugin"; // optional module docstring

  m.def("read_performance_polar_set", [](const std::string &nc_polar){
    return read_performance_polar_set(nc_polar);
    });

  py::class_<PerformancePolarSet, std::shared_ptr<PerformancePolarSet>>(m, "PerformancePolarSet")
      .def("name", &PerformancePolarSet::name)
      // .def("exist", &PerformancePolarSet::exist) // smichel bug to fix
      .def("polar_set_list", &PerformancePolarSet::polar_set_list)
      .def("polar_set", &PerformancePolarSet::polar_set);

  py::class_<PolarSet, std::shared_ptr<PolarSet>>(m, "PolarSet")
      .def("name", &PolarSet::name)
      // .def("polar_type", &PolarSet::polar_type) // smichel commented for now, need to know POLAR_TYPE class
      .def("polar_type_str", &PolarSet::polar_type_str)
      .def("polar_names", &PolarSet::polar_names)
      .def("get_5D_polar", &PolarSet::polar<double, 5>)
      .def("get_3D_polar", &PolarSet::polar<double, 3>); 

  using Interp5DPolar = InterpolablePolar<5>;
  using Shared5DPolar = std::shared_ptr<InterpolablePolar<5>>;
  py::class_<Interp5DPolar, Shared5DPolar>(m, "InterpolablePolar5D") // Daughter class from Polar, PolarBase, Named
      .def("dim", &Interp5DPolar::dim)
      .def("size", &Interp5DPolar::size)
      // .def("polar_type", &Interp3DPolar::polar_type) // smichel commented for now, need to know POLAR_TYPE class
      .def("is_filled", &Interp5DPolar::is_filled)
      // .def("dimension_set", &Interp5DPolar::dimension_set) // smichel commented for now, not needed in python    
      .def("dimension_set_names", &Interp5DPolar::dimension_set_names) 
      .def("coordinates", &Interp5DPolar::coordinates)
      .def("min_bounds", &Interp5DPolar::min_bounds)
      .def("max_bounds", &Interp5DPolar::max_bounds)    
      .def("min_value", &Interp5DPolar::min_value) 
      .def("max_value", &Interp5DPolar::max_value) 
      .def("nearest", &Interp5DPolar::nearest) 
      .def("interp", &Interp5DPolar::interp);
      
  using Interp3DPolar = InterpolablePolar<3>;
  using Shared3DPolar = std::shared_ptr<InterpolablePolar<3>>;
  py::class_<Interp3DPolar, Shared3DPolar>(m, "InterpolablePolar3D") // Daughter class from Polar, PolarBase, Named
      .def("dim", &Interp3DPolar::dim)
      .def("size", &Interp3DPolar::size)
      // .def("polar_type", &Interp3DPolar::polar_type) // smichel commented for now, need to know POLAR_TYPE class
      .def("is_filled", &Interp3DPolar::is_filled)
      // .def("dimension_set", &Interp3DPolar::dimension_set) // smichel commented for now, not needed in python    
      .def("dimension_set_names", &Interp3DPolar::dimension_set_names) 
      .def("coordinates", &Interp3DPolar::coordinates)
      .def("min_bounds", &Interp3DPolar::min_bounds)
      .def("max_bounds", &Interp3DPolar::max_bounds)  
      .def("min_value", &Interp3DPolar::min_value) 
      .def("max_value", &Interp3DPolar::max_value)    
      .def("nearest", &Interp3DPolar::nearest) 
      .def("interp", &Interp3DPolar::interp);
}