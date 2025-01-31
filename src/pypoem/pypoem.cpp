//
// Created by frongere on 27/01/25.
//

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <poem/poem.h>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

namespace py = pybind11;
using namespace poem;


PYBIND11_MODULE(pypoem, m) {

  m.doc() = R"pbdoc(Performance pOlar Exchange forMat)pbdoc";

  m.def("current_standard_poem_version", &current_poem_standard_version,
        R"pbdoc(Get the current specifications number of the library)pbdoc");

  // Dimension
  py::class_<Dimension, std::shared_ptr<Dimension>> Dimension(m, "Dimension");
  Dimension.doc() = R"pbdoc("")pbdoc";
  Dimension.def(py::init<const std::string &, const std::string &, const std::string &>())
  Dimension.def("name", &Dimension::name);


  m.def("make_dimension", &make_dimension,
        R"pbdoc("Build a Dimension")pbdoc");


  // DimensionSet
  py::class_<DimensionSet, std::shared_ptr<DimensionSet>>(m, "DimensionSet")
      .def(py::init<std::vector<std::shared_ptr<Dimension>>>());
  m.def("make_dimension_set", &make_dimension_set,
        R"pbdoc(Build a DimensionSet)pbdoc");

  // DimensionPoint
  py::class_<DimensionPoint>(m, "DimensionPoint")
      .def(py::init<std::shared_ptr<DimensionSet>>(),
           R"pbdoc()pbdoc")
      .def("get", py::overload_cast<const std::string &>(&DimensionPoint::get),
           R"pbdoc()pbdoc");

  // DimensionGrid
  py::class_<DimensionGrid, std::shared_ptr<DimensionGrid>>(m, "DimensionGrid")
      .def("set_values", &DimensionGrid::set_values,
           R"pbdoc()pbdoc")
      .def("values", py::overload_cast<const std::string &>(&DimensionGrid::values, py::const_),
           R"pbdoc()pbdoc")
      .def("dimension_points", &DimensionGrid::dimension_points,
           R"pbdoc()pbdoc");

  m.def("make_dimension_grid", &make_dimension_grid,
        R"pbdoc()pbdoc");


  py::enum_<POEM_DATATYPE>(m, "POEM_DATATYPE")
      .value("POEM_DOUBLE", POEM_DOUBLE,
             R"pbdoc()pbdoc")
      .value("POEM_INT", POEM_INT,
             R"pbdoc()pbdoc")
      .export_values();


  // PolarNode
  // TODO

  py::class_<PolarNode, std::shared_ptr<PolarNode>>(m, "PolarNode")
      .def(py::init<const std::string &>(),
           R"pbdoc()pbdoc");

  // PolarTable
//  py::class_<PolarTableBase, std::shared_ptr<PolarTableBase>, PolarNode>(m, "PolarTable");
//      .def(py::init<const std::string &, const std::string &, const std::string &,
//          POEM_DATATYPE, std::shared_ptr<DimensionGrid>>()); // FIXME: PolarTableBase is abstract

  py::class_<PolarTable<double>, std::shared_ptr<PolarTable<double>>, PolarNode>(m, "PolarTableDouble")
      .def("fill_with", &PolarTable<double>::fill_with,
           R"pbdoc()pbdoc");
  py::class_<PolarTable<int>, std::shared_ptr<PolarTable<int>>, PolarNode>(m, "PolarTableInt")
      .def("fill_with", &PolarTable<int>::fill_with,
           R"pbdoc()pbdoc");

  m.def("make_polar_table_double", &make_polar_table_double,
        R"pbdoc()pbdoc");
  m.def("make_polar_table_int", &make_polar_table_int,
        R"pbdoc()pbdoc");

  // Polar
  py::enum_<POLAR_MODE>(m, "POLAR_MODE")
      .value("MPPP", MPPP,
             R"pbdoc()pbdoc")
      .value("HPPP", HPPP,
             R"pbdoc()pbdoc")
      .value("MVPP", MVPP,
             R"pbdoc()pbdoc")
      .value("HVPP", HVPP,
             R"pbdoc()pbdoc")
      .value("VPP", VPP,
             R"pbdoc()pbdoc")
      .export_values();

  py::class_<PolarSet, std::shared_ptr<PolarSet>, PolarNode>(m, "PolarSet")
      .def(py::init<const std::string &>())
      .def("attach_polar", &PolarSet::attach_polar,
           R"pbdoc()pbdoc");

  m.def("make_polar_set", &make_polar_set,
        R"pbdoc()pbdoc");


  py::class_<Polar, std::shared_ptr<Polar>, PolarNode>(m, "Polar")
      .def(py::init<const std::string &, POLAR_MODE, std::shared_ptr<DimensionGrid>>(),
           R"pbdoc()pbdoc")
      .def("create_polar_table_double", &Polar::create_polar_table<double>,
           R"pbdoc()pbdoc")
      .def("create_polar_table_int", &Polar::create_polar_table<int>,
           R"pbdoc()pbdoc");

  m.def("make_polar", &make_polar,
        R"pbdoc()pbdoc");


  m.def("to_netcdf", [](std::shared_ptr<PolarNode> polar_node, const std::string &filename) -> void {
          to_netcdf(polar_node, filename);
        },
        R"pbdoc()pbdoc");


}

/*
 * On veut pouvoir
 * - construire arboresence de PolarNode
 * - construire PolarSet
 * - construie Polar
 * - construire PolarTable
 * - remplir PolarTable
 * - Specifier des attributs
 * - Ecrire un PolarNode
 * - Lire un fichier de Polaire
 * - effectuer un check de validite sur une polaire
 * - recuperer version poem d'une polaire
 * -
 */
