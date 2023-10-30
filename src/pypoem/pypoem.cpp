//
// Created by hippolyte on 26/10/23.
//

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "PyWrapper.h"

namespace py = pybind11;

PYBIND11_MODULE(pypoem, m) {
  m.doc() = "python bindings for poem.";
  py::register_exception<poem::PoemException>(m, "PoemError");

  py::class_<PyWrapper>(m, "Polar")
      .def(py::init<std::string>(), py::arg("path"))
      .def("get_names", &PyWrapper::get_names)
      .def("get_attributes", &PyWrapper::get_attributes);
}