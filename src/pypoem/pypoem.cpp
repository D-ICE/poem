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
    return *read_performance_polar_set(nc_polar);
    });

  py::class_<PerformancePolarSet>(m, "PerformancePolarSet")
      // .def(py::init<Attributes>()) // smichel not used for now, else wrap also the Attributes class
      .def("name", &PerformancePolarSet::name)
      // .def("exist", [](const std::string &ne)  { // smichel bug to fix
      //   return PerformancePolarSet::exist(ne);
      //   })
      .def("polar_set_list", &PerformancePolarSet::polar_set_list)
      .def("polar_set", &PerformancePolarSet::polar_set);

  py::class_<PolarSet>(m, "PolarSet")
    //   .def(py::init<>())
      .def("name", &PolarSet::name)
      .def("polar_type", &PolarSet::polar_type)
      .def("polar_type_str", &PolarSet::polar_type_str)
      .def("polar_names", &PolarSet::polar_names)
      .def("set_5D_polar", &PolarSet::polar<double, 5>)
      .def("set_3D_polar", &PolarSet::polar<double, 3>); 

  using My5DPolar = InterpolablePolar<5>;
  py::class_<My5DPolar>(m, "InterpolablePolar5D") // Daughter class from Polar, PolarBase, Named
    //   .def(py::init<>())
      .def("dim", &My5DPolar::dim)
      .def("size", &My5DPolar::size)
      .def("polar_type", &My5DPolar::polar_type)
      .def("is_filled", &My5DPolar::is_filled)
      .def("get_dimension_set", &My5DPolar::get_dimension_set)     
      .def("get_dimension_set_names", &My5DPolar::get_dimension_set_names) 
      .def("min_bounds", &My5DPolar::min_bounds)
      .def("max_bounds", &My5DPolar::max_bounds)     
      .def("nearest", &My5DPolar::nearest) 
      .def("interp", &My5DPolar::interp);
      
  using My3DPolar = InterpolablePolar<3>;
  py::class_<My3DPolar>(m, "InterpolablePolar3D") // Daughter class from Polar, PolarBase, Named
    //   .def(py::init<>())
      .def("dim", &My3DPolar::dim)
      .def("size", &My3DPolar::size)
      .def("polar_type", &My3DPolar::polar_type)
      .def("is_filled", &My3DPolar::is_filled)
      .def("get_dimension_set", &My3DPolar::get_dimension_set)     
      .def("get_dimension_set_names", &My3DPolar::get_dimension_set_names) 
      .def("min_bounds", &My3DPolar::min_bounds)
      .def("max_bounds", &My3DPolar::max_bounds)     
      .def("nearest", &My3DPolar::nearest) 
      .def("interp", &My3DPolar::interp);
}