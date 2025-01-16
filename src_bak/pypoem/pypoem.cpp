#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <string>
#include "poem/poem.h"

namespace py = pybind11;

PYBIND11_MODULE(pypoem, m) {
  using namespace poem;

  // m.doc() = "pybind11 example plugin"; // optional module docstring

  m.def("read_polar_set", [](const std::string &nc_polar){
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

  using Interp5DPolarTable = InterpolablePolarTable<5>;
  using Shared5DPolarTable = std::shared_ptr<InterpolablePolarTable<5>>;
  py::class_<Interp5DPolarTable, Shared5DPolarTable>(m, "InterpolablePolarTable5D") // Daughter class from Polar, PolarTableBase, Named
      .def("dim", &Interp5DPolarTable::dim)
      .def("size", &Interp5DPolarTable::size)
      .def("is_filled", &Interp5DPolarTable::is_filled)
      .def("dimension_set_names", &Interp5DPolarTable::dimension_set_names)
//      .def("coordinates", &Interp5DPolarTable::coordinates)
      .def("min_bound", &Interp5DPolarTable::min_bound)
      .def("max_bound", &Interp5DPolarTable::max_bound)
      .def("min_value", &Interp5DPolarTable::min_value)
      .def("max_value", &Interp5DPolarTable::max_value)
      .def("nearest", &Interp5DPolarTable::nearest)
      .def("interp", &Interp5DPolarTable::interp);
      
  using Interp4DPolarTable = InterpolablePolarTable<4>;
  using Shared4DPolarTable = std::shared_ptr<InterpolablePolarTable<4>>;
  py::class_<Interp4DPolarTable, Shared4DPolarTable>(m, "InterpolablePolarTable4D") // Daughter class from Polar, PolarTableBase, Named
      .def("dim", &Interp4DPolarTable::dim)
      .def("size", &Interp4DPolarTable::size)
      .def("is_filled", &Interp4DPolarTable::is_filled)
      .def("dimension_set_names", &Interp4DPolarTable::dimension_set_names)
//      .def("coordinates", &Interp4DPolarTable::coordinates)
      .def("min_bound", &Interp4DPolarTable::min_bound)
      .def("max_bound", &Interp4DPolarTable::max_bound)
      .def("min_value", &Interp4DPolarTable::min_value)
      .def("max_value", &Interp4DPolarTable::max_value)
      .def("nearest", &Interp4DPolarTable::nearest)
      .def("interp", &Interp4DPolarTable::interp);
      
  using Interp3DPolarTable = InterpolablePolarTable<3>;
  using Shared3DPolarTable = std::shared_ptr<InterpolablePolarTable<3>>;
  py::class_<Interp3DPolarTable, Shared3DPolarTable>(m, "InterpolablePolarTable3D") // Daughter class from Polar, PolarTableBase, Named
      .def("dim", &Interp3DPolarTable::dim)
      .def("size", &Interp3DPolarTable::size)
      .def("is_filled", &Interp3DPolarTable::is_filled)
      .def("dimension_set_names", &Interp3DPolarTable::dimension_set_names)
//      .def("coordinates", &Interp3DPolarTable::coordinates)
      .def("min_bound", &Interp3DPolarTable::min_bound)
      .def("max_bound", &Interp3DPolarTable::max_bound)
      .def("min_value", &Interp3DPolarTable::min_value)
      .def("max_value", &Interp3DPolarTable::max_value)
      .def("nearest", &Interp3DPolarTable::nearest)
      .def("interp", &Interp3DPolarTable::interp);
}