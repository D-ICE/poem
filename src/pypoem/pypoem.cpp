//
// Created by frongere on 27/01/25.
//

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <poem/poem.h>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

namespace py = pybind11;
//using namespace poem;


PYBIND11_MODULE(pypoem, m) {

  m.doc() = R"pbdoc(Performance pOlar Exchange forMat)pbdoc";

  m.def("current_standard_poem_version", &poem::current_poem_standard_version,
        R"pbdoc(Get the current specifications number of the library)pbdoc");

  // Dimension
  py::class_<poem::Dimension, std::shared_ptr<poem::Dimension>> Dimension(m, "Dimension");
  Dimension.doc() = R"pbdoc("")pbdoc";
  Dimension.def(py::init<const std::string &, const std::string &, const std::string &>());
  Dimension.def("name", &poem::Dimension::name);


  m.def("make_dimension", &poem::make_dimension,
        R"pbdoc("Build a Dimension")pbdoc");


  // DimensionSet
  py::class_<poem::DimensionSet, std::shared_ptr<poem::DimensionSet>>(m, "DimensionSet")
      .def(py::init<std::vector<std::shared_ptr<poem::Dimension>>>());
  m.def("make_dimension_set", &poem::make_dimension_set,
        R"pbdoc(Build a DimensionSet)pbdoc");

  // DimensionPoint
  py::class_<poem::DimensionPoint>(m, "DimensionPoint")
      .def(py::init<std::shared_ptr<poem::DimensionSet>>(),
           R"pbdoc()pbdoc")
      .def("get", py::overload_cast<const std::string &>(&poem::DimensionPoint::get),
           R"pbdoc()pbdoc");

  // DimensionGrid
  py::class_<poem::DimensionGrid, std::shared_ptr<poem::DimensionGrid>>(m, "DimensionGrid")
      .def("set_values", &poem::DimensionGrid::set_values,
           R"pbdoc()pbdoc")
      .def("values", py::overload_cast<const std::string &>(&poem::DimensionGrid::values, py::const_),
           R"pbdoc()pbdoc")
      .def("dimension_points", &poem::DimensionGrid::dimension_points,
           R"pbdoc()pbdoc");

  m.def("make_dimension_grid", &poem::make_dimension_grid,
        R"pbdoc()pbdoc");


  py::enum_<poem::POEM_DATATYPE>(m, "POEM_DATATYPE")
      .value("POEM_DOUBLE", poem::POEM_DOUBLE,
             R"pbdoc()pbdoc")
      .value("POEM_INT", poem::POEM_INT,
             R"pbdoc()pbdoc")
      .export_values();


  // PolarNode
  // TODO

  py::class_<poem::PolarNode, std::shared_ptr<poem::PolarNode>>(m, "PolarNode")
      .def(py::init<const std::string &>(),
           R"pbdoc()pbdoc");

  // PolarTable
//  py::class_<PolarTableBase, std::shared_ptr<PolarTableBase>, PolarNode>(m, "PolarTable");
//      .def(py::init<const std::string &, const std::string &, const std::string &,
//          POEM_DATATYPE, std::shared_ptr<DimensionGrid>>()); // FIXME: PolarTableBase is abstract

  py::class_<poem::PolarTable<double>, std::shared_ptr<poem::PolarTable<double>>, poem::PolarNode>(m, "PolarTableDouble")
      .def("fill_with", &poem::PolarTable<double>::fill_with,
           R"pbdoc()pbdoc");
  py::class_<poem::PolarTable<int>, std::shared_ptr<poem::PolarTable<int>>, poem::PolarNode>(m, "PolarTableInt")
      .def("fill_with", &poem::PolarTable<int>::fill_with,
           R"pbdoc()pbdoc");

  m.def("make_polar_table_double", &poem::make_polar_table_double,
        R"pbdoc()pbdoc");
  m.def("make_polar_table_int", &poem::make_polar_table_int,
        R"pbdoc()pbdoc");

  // Polar
  py::enum_<poem::POLAR_MODE>(m, "POLAR_MODE")
      .value("MPPP", poem::MPPP,
             R"pbdoc()pbdoc")
      .value("HPPP", poem::HPPP,
             R"pbdoc()pbdoc")
      .value("MVPP", poem::MVPP,
             R"pbdoc()pbdoc")
      .value("HVPP", poem::HVPP,
             R"pbdoc()pbdoc")
      .value("VPP", poem::VPP,
             R"pbdoc()pbdoc")
      .export_values();

  py::class_<poem::PolarSet, std::shared_ptr<poem::PolarSet>, poem::PolarNode>(m, "PolarSet")
      .def(py::init<const std::string &>())
      .def("attach_polar", &poem::PolarSet::attach_polar,
           R"pbdoc()pbdoc");

  m.def("make_polar_set", &poem::make_polar_set,
        R"pbdoc()pbdoc");


  py::class_<poem::Polar, std::shared_ptr<poem::Polar>, poem::PolarNode>(m, "Polar")
      .def(py::init<const std::string &, poem::POLAR_MODE, std::shared_ptr<poem::DimensionGrid>>(),
           R"pbdoc()pbdoc")
      .def("create_polar_table_double", &poem::Polar::create_polar_table<double>,
           R"pbdoc()pbdoc")
      .def("create_polar_table_int", &poem::Polar::create_polar_table<int>,
           R"pbdoc()pbdoc");

  m.def("make_polar", &poem::make_polar,
        R"pbdoc()pbdoc");


  m.def("to_netcdf", [](std::shared_ptr<poem::PolarNode> polar_node, const std::string &filename) -> void {
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
