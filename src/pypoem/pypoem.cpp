//
// Created by hippolyte on 26/10/23.
//

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "poem/poem.h"

#include "PyWrapper.h"

namespace py = pybind11;

PYBIND11_MODULE(_pypoem, m) {
  m.doc() = "python bindings for poem.";
  py::register_exception<poem::PoemException>(m, "PoemError");

  py::class_<PyWrapper>(m, "PolarSetReader")
      .def(py::init<std::string>(), py::arg("path"))
      .def_property_readonly("polar_set", &PyWrapper::get_set)
      .def_property_readonly("attributes", &PyWrapper::get_attributes);

  py::class_<poem::PolarSet, std::shared_ptr<poem::PolarSet>>(m, "PolarSet")
      .def("polar_names", &poem::PolarSet::polar_names);
}