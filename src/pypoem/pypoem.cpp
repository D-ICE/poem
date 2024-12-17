#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <string>
#include "poem/poem.h"

namespace py = pybind11;

PYBIND11_MODULE(pypoem, m) {
  using namespace poem;

  // m.doc() = "pybind11 example plugin"; // optional module docstring

  m.def("read_polar", [](const std::string &nc_polar){
    return read_polar_set(nc_polar);
    });

  py::class_<PolarSet, std::shared_ptr<PolarSet>>(m, "PolarSet")
      .def("name", &PolarSet::name)
      // .def("exist", &PolarSet::exist) // smichel bug to fix
      .def("polar_list", &PolarSet::polar_list)
      .def("polar", &PolarSet::polar);

  py::class_<Polar, std::shared_ptr<Polar>>(m, "Polar")
      .def("name", &Polar::name)
      // .def("polar_type", &Polar::polar_type) // smichel commented for now, need to know POLAR_TYPE class
      .def("polar_type_str", &Polar::polar_type_str)
      .def("polar_names", &Polar::polar_names)
      .def("get_5D_polar", &Polar::polar_table<double, 5>)
      .def("get_4D_polar", &Polar::polar_table<double, 4>)
      .def("get_3D_polar", &Polar::polar_table<double, 3>);

  using Interp5DPolar = InterpolablePolarTable<5>;
  using Shared5DPolar = std::shared_ptr<InterpolablePolarTable<5>>;
  py::class_<Interp5DPolar, Shared5DPolar>(m, "InterpolablePolar5D") // Daughter class from Polar, PolarTableBase, Named
      .def("dim", &Interp5DPolar::dim)
      .def("size", &Interp5DPolar::size)
      .def("is_filled", &Interp5DPolar::is_filled)
      .def("dimension_set_names", &Interp5DPolar::dimension_set_names) 
      .def("coordinates", &Interp5DPolar::coordinates)
      .def("min_bounds", &Interp5DPolar::min_bounds)
      .def("max_bounds", &Interp5DPolar::max_bounds)    
      .def("min_value", &Interp5DPolar::min_value) 
      .def("max_value", &Interp5DPolar::max_value) 
      .def("nearest", &Interp5DPolar::nearest) 
      .def("interp", &Interp5DPolar::interp);
      
  using Interp4DPolar = InterpolablePolarTable<4>;
  using Shared4DPolar = std::shared_ptr<InterpolablePolarTable<4>>;
  py::class_<Interp4DPolar, Shared4DPolar>(m, "InterpolablePolar4D") // Daughter class from Polar, PolarTableBase, Named
      .def("dim", &Interp4DPolar::dim)
      .def("size", &Interp4DPolar::size)
      .def("is_filled", &Interp4DPolar::is_filled)
      .def("dimension_set_names", &Interp4DPolar::dimension_set_names) 
      .def("coordinates", &Interp4DPolar::coordinates)
      .def("min_bounds", &Interp4DPolar::min_bounds)
      .def("max_bounds", &Interp4DPolar::max_bounds)    
      .def("min_value", &Interp4DPolar::min_value) 
      .def("max_value", &Interp4DPolar::max_value) 
      .def("nearest", &Interp4DPolar::nearest) 
      .def("interp", &Interp4DPolar::interp);
      
  using Interp3DPolar = InterpolablePolarTable<3>;
  using Shared3DPolar = std::shared_ptr<InterpolablePolarTable<3>>;
  py::class_<Interp3DPolar, Shared3DPolar>(m, "InterpolablePolar3D") // Daughter class from Polar, PolarTableBase, Named
      .def("dim", &Interp3DPolar::dim)
      .def("size", &Interp3DPolar::size)
      .def("is_filled", &Interp3DPolar::is_filled)
      .def("dimension_set_names", &Interp3DPolar::dimension_set_names) 
      .def("coordinates", &Interp3DPolar::coordinates)
      .def("min_bounds", &Interp3DPolar::min_bounds)
      .def("max_bounds", &Interp3DPolar::max_bounds)  
      .def("min_value", &Interp3DPolar::min_value) 
      .def("max_value", &Interp3DPolar::max_value)    
      .def("nearest", &Interp3DPolar::nearest) 
      .def("interp", &Interp3DPolar::interp);
}