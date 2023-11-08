//
// Created by hippolyte on 26/10/23.
//

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "PyWrapper.h"
#include "poem/poem.h"

namespace py = pybind11;

template <size_t dim>
void declare_interpolable_polar(py::module &m) {
  using InterpPolar = poem::InterpolablePolar<dim>;

  std::string pyclass_name = std::string("InterpolablePolar") +
                             std::to_string((int)dim) + std::string("D");
  py::class_<InterpPolar>(m, pyclass_name.c_str())
      .def("interpolate", &InterpPolar::interp);
}

PYBIND11_MODULE(_pypoem, m) {
  m.doc() = "python bindings for poem.";
  py::register_exception<poem::PoemException>(m, "PoemError");

  py::class_<PyWrapper>(m, "PolarSetReader")
      .def(py::init<std::string>(), py::arg("path"))
      .def_property_readonly("polar_set", &PyWrapper::get_set,
                             py::return_value_policy::reference)
      .def_property_readonly("attributes", &PyWrapper::get_attributes);

  py::class_<poem::PolarSet, std::shared_ptr<poem::PolarSet>>(m, "_PolarSet")
      .def_property_readonly("polar_names", &poem::PolarSet::polar_names)
      .def("get", [](const poem::PolarSet &self, std::string &name) {
        static_cast<poem::PolarBase *>(self.polar(name));
      });

  //    declare_interpolable_polar<4>(m);
  //    declare_interpolable_polar<5>(m);

  py::class_<poem::PolarBase>(m, "_Polar")
      .def("interp4d", &poem::InterpolablePolar<4>::interp)
      .def("interp5d", &poem::InterpolablePolar<5>::interp)
      .def("nearest4i", &poem::Polar<int, 4>::nearest)
      .def("nearest5i", &poem::Polar<int, 5>::nearest);
}