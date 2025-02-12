//
// Created by frongere on 27/01/25.
//

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include "poem/poem.h"

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

namespace py = pybind11;
using namespace pybind11::literals;

// https://stackoverflow.com/questions/76235970/pybind11-convert-arbitrary-numpy-arrays-to-vectors

// ===================================================================================================================
// Utility functions
// ===================================================================================================================

template<typename T>
inline std::vector<T> ndarray2stdvector(const py::array_t<double, py::array::c_style | py::array::forcecast> &array) {
  std::vector<T> vector(array.size());
  std::memcpy(vector.data(), array.data(), array.size() * sizeof(T));
  return vector;
}

template<typename T>
inline py::memoryview vector2memoryview(std::vector<T> &vector, const std::vector<size_t> &shape) {
  size_t ndims = shape.size();
  std::vector<py::ssize_t> strides(ndims);
  strides[ndims - 1] = sizeof(double);

  size_t n = 0;
  for (size_t i = 2; i <= ndims; ++i) {
    strides[ndims - i] = py::ssize_t_cast(shape[ndims - i + 1] * strides[ndims - i + 1]);
    n++;
  }

  return py::memoryview::from_buffer(
      vector.data(),
      shape,
      strides
  );
}


// ===================================================================================================================
// Python module definition
// ===================================================================================================================

PYBIND11_MODULE(pypoem, m) {

  // ===================================================================================================================
  // Module level
  // ===================================================================================================================
  m.doc() = R"pbdoc(Performance pOlar Exchange forMat)pbdoc";

  m.def("current_standard_poem_version", &poem::current_poem_standard_version,
        R"pbdoc(Get the current specifications number of the library)pbdoc");

  // ===================================================================================================================
  // Dimension
  // ===================================================================================================================
  py::class_<poem::Dimension, std::shared_ptr<poem::Dimension>> Dimension(m, "Dimension");
  Dimension.doc() = R"pbdoc(A Dimension is a named coordinate for a DimensionGrid)pbdoc";
  Dimension.def(py::init<const std::string &, const std::string &, const std::string &>());
  Dimension.def("name", &poem::Dimension::name, R"pbdoc(Get the name of the Dimension)pbdoc");

  m.def("make_dimension", &poem::make_dimension,
        R"pbdoc(Build a Dimension)pbdoc",
        "name"_a, "unit"_a, "description"_a);

  // ===================================================================================================================
  // DimensionSet
  // ===================================================================================================================
  py::class_<poem::DimensionSet, std::shared_ptr<poem::DimensionSet>> DimensionSet(m, "DimensionSet");
  DimensionSet.doc() =
      R"pbdoc(A DimensionSet is an ordered set of Dimensions that define the Coordinate system for a DimensionGrid)pbdoc";
  DimensionSet.def(py::init<std::vector<std::shared_ptr<poem::Dimension>>>());

  m.def("make_dimension_set", &poem::make_dimension_set,
        R"pbdoc(Build a DimensionSet)pbdoc",
        "dimension_vector"_a);

  // ===================================================================================================================
  // DimensionGrid
  // ===================================================================================================================
  py::class_<poem::DimensionGrid, std::shared_ptr<poem::DimensionGrid>> DimensionGrid(m, "DimensionGrid");
  DimensionGrid.doc() = R"pbdoc("A DimensionGrid is a numerical realisation of a DimensionSet.
Each Dimension of the associated DimensionSet is given a numerical sampling.")pbdoc";
  DimensionGrid.def("set_values", &poem::DimensionGrid::set_values,
                    R"pbdoc("Set a values vector for the specified Dimension")pbdoc",
                    "dimension_name"_a, "sampling_vector"_a);
  DimensionGrid.def("values",
                    py::overload_cast<const std::string &>(&poem::DimensionGrid::values, py::const_),
                    R"pbdoc("Get the values vector for the specified Dimension")pbdoc",
                    "dimension_name"_a);
  DimensionGrid.def("dimension_points", &poem::DimensionGrid::dimension_points,
                    R"pbdoc("Get the DimensionPoint vector for the DimensionGrid")pbdoc");
  DimensionGrid.def("size",
                    py::overload_cast<const std::string &>(&poem::DimensionGrid::size, py::const_),
                    R"pbdoc("Get the size of the specified Dimension")pbdoc",
                    "name"_a);
  DimensionGrid.def("shape", &poem::DimensionGrid::shape,
                    R"pbdoc(Returns an ordered list of Dimension Sizes)pbdoc");

  m.def("make_dimension_grid", &poem::make_dimension_grid,
        R"pbdoc()pbdoc");

  // ===================================================================================================================
  // DimensionPoint
  // ===================================================================================================================
  py::class_<poem::DimensionPoint> DimensionPoint(m, "DimensionPoint");
  DimensionPoint.doc() = R"pbdoc("A DimensionPoint is a point inside a DimensinoGrid with cartesian coordinates")pbdoc";
  DimensionPoint.def(py::init<std::shared_ptr<poem::DimensionSet>>(),
                     R"pbdoc()pbdoc");
  DimensionPoint.def("get",
                     py::overload_cast<const std::string &>(&poem::DimensionPoint::get),
                     R"pbdoc()pbdoc");

  // ===================================================================================================================
  // Enums
  // ===================================================================================================================

  py::enum_<poem::POEM_DATATYPE> POEM_DATATYPE(m, "POEM_DATATYPE");
//  POEM_DATATYPE.doc() = R"pbdoc("Datatype for PolarTables")pbdoc";
  POEM_DATATYPE.value("POEM_DOUBLE", poem::POEM_DOUBLE,
                      R"pbdoc(double datatype)pbdoc");
  POEM_DATATYPE.value("POEM_INT", poem::POEM_INT,
                      R"pbdoc(int datatype)pbdoc");
  POEM_DATATYPE.export_values();

  py::enum_<poem::POLAR_MODE> POLAR_MODE(m, "POLAR_MODE");
//  POLAR_MODE.doc() =
//      R"pbdoc("A POLAR_MODE is a specific type of POLAR that define the type prediction used to build the polar data")pbdoc";
  POLAR_MODE.value("MPPP", poem::MPPP,
                   R"pbdoc(Motor only, Power Prediction Program)pbdoc");
  POLAR_MODE.value("HPPP", poem::HPPP,
                   R"pbdoc(Hybrid motor and wind propulsion, Power Prediction Program)pbdoc");
  POLAR_MODE.value("MVPP", poem::MVPP,
                   R"pbdoc(Motor only, Velocity Prediction Program)pbdoc");
  POLAR_MODE.value("HVPP", poem::HVPP,
                   R"pbdoc(Hybrid motor and wind propulsion, Velocity Prediction Program)pbdoc");
  POLAR_MODE.value("VPP", poem::VPP,
                   R"pbdoc(wind propulsion only, Velocity Prediction Program)pbdoc");
  POLAR_MODE.export_values();

  // ===================================================================================================================
  // PolarNode
  // ===================================================================================================================
  py::class_<poem::PolarNode, std::shared_ptr<poem::PolarNode>> PolarNode(m, "PolarNode");
  PolarNode.doc() = R"pbdoc("A PolarNode is the generic type for tree-structured Polar")pbdoc";
  PolarNode.def(py::init<const std::string &>());

  // ===================================================================================================================
  // PolarTable<T>
  // ===================================================================================================================

  // -------------------------------------------- PolarTableDouble -----------------------------------------------------
  py::class_<poem::PolarTable<double>, std::shared_ptr<poem::PolarTable<double>>, poem::PolarNode>
      PolarTableDouble(m, "PolarTableDouble");
  PolarTableDouble.doc() =
      R"pbdoc("A PolarTableDouble is a special PolarNode used to represent a specific variable
               stored in a multidimensional array. Double version.")pbdoc";
  PolarTableDouble.def("fill_with", &poem::PolarTable<double>::fill_with,
                       R"pbdoc()pbdoc", "value"_a);
  PolarTableDouble.def("set_values",
                       [](poem::PolarTable<double> &self,
                          const py::array_t<double, py::array::c_style | py::array::forcecast> &array) -> void {
                         self.set_values(ndarray2stdvector<double>(array));
                       },
                       R"pbdoc()pbdoc");
  PolarTableDouble.def("set_value",
                       py::overload_cast<std::vector<size_t>, const double &>(&poem::PolarTable<double>::set_value));

//
  PolarTableDouble.def("array",
                       [](poem::PolarTable<double> &self) -> py::array_t<double> {
                         return vector2memoryview(self.values(), self.dimension_grid()->shape());
                       },
                       R"pbdoc(Returns the PolarTable NDArray (no copy))pbdoc");
  PolarTableDouble.def("dimension_grid", &poem::PolarTable<double>::dimension_grid,
                       R"pbdoc(Returns the DimensionGrid associated to the PolarTable)pbdoc");


  m.def("make_polar_table_double", &poem::make_polar_table_double,
        R"pbdoc()pbdoc",
        "name"_a, "unit"_a, "description"_a, "dimension_grid"_a);

  // -------------------------------------------- PolarTableInt -----------------------------------------------------
  py::class_<poem::PolarTable<int>, std::shared_ptr<poem::PolarTable<int>>, poem::PolarNode>
      PolarTableInt(m, "PolarTableInt");
  PolarTableInt.doc() = R"pbdoc("A PolarTableInt is a special PolarNode used to represent a specific variable
stored in a multidimensional array. Int version.")pbdoc";
  PolarTableInt.def("fill_with", &poem::PolarTable<int>::fill_with,
                    R"pbdoc()pbdoc", "value"_a);
  PolarTableDouble.def("set_values",
                       [](poem::PolarTable<int> &self,
                          const py::array_t<int, py::array::c_style | py::array::forcecast> &array) -> void {
                         self.set_values(ndarray2stdvector<int>(array));
                       },
                       R"pbdoc()pbdoc");
  PolarTableDouble.def("array",
                       [](poem::PolarTable<int> &self) -> py::array_t<int> {
                         return vector2memoryview(self.values(), self.dimension_grid()->shape());
                       },
                       R"pbdoc(Returns the PolarTable NDArray (no copy))pbdoc");


  m.def("make_polar_table_int", &poem::make_polar_table_int,
        R"pbdoc()pbdoc"
        "name"_a, "unit"_a, "description"_a, "dimension_grid"_a);


  // ===================================================================================================================
  // Polar
  // ===================================================================================================================
  py::class_<poem::Polar, std::shared_ptr<poem::Polar>, poem::PolarNode> Polar(m, "Polar");
  Polar.doc() = R"pbdoc("A Polar is a special PolarNode used to group PolarTables relative to a specific POLAR_MODE")pbdoc";
  Polar.def(py::init<const std::string &, poem::POLAR_MODE, std::shared_ptr<poem::DimensionGrid>>(),
            R"pbdoc()pbdoc"); // &poem::Polar::create_polar_table<double>
  Polar.def("create_polar_table_double", [](poem::Polar &self,
                                            const std::string &name,
                                            const std::string &unit,
                                            const std::string &description) -> std::shared_ptr<poem::PolarTable<double>> {
              return self.create_polar_table<double>(name, unit, description, poem::POEM_DOUBLE);
            },
            R"pbdoc("Create a new PolarTable with type double from a Polar")pbdoc");
  Polar.def("create_polar_table_int", [](poem::Polar &self,
                                         const std::string &name,
                                         const std::string &unit,
                                         const std::string &description) -> std::shared_ptr<poem::PolarTable<int>> {
              return self.create_polar_table<int>(name, unit, description, poem::POEM_INT);
            },
            R"pbdoc("Create a new PolarTable with type int from a Polar")pbdoc");

  m.def("make_polar", &poem::make_polar,
        R"pbdoc("Make a Polar")pbdoc",
        "name"_a, "polar_mode"_a, "dimension_grid"_a);

  // ===================================================================================================================
  // PolarSet
  // ===================================================================================================================
  py::class_<poem::PolarSet, std::shared_ptr<poem::PolarSet>, poem::PolarNode> PolarSet(m, "PolarSet");
  PolarSet.doc() = R"pbdoc("A PolarSet is a special PolarNode used to group a set of Polar")pbdoc";
  PolarSet.def(py::init<const std::string &>());
  PolarSet.def("attach_polar", &poem::PolarSet::attach_polar,
               R"pbdoc()pbdoc",
               "polar"_a);

  m.def("make_polar_set", &poem::make_polar_set,
        R"pbdoc()pbdoc",
        "name"_a);

  // ===================================================================================================================
  // Writer
  // ===================================================================================================================
  m.def("to_netcdf", [](std::shared_ptr<poem::PolarNode> polar_node,
                        const std::string &vessel_name,
                        const std::string &filename) -> void {
          poem::to_netcdf(polar_node, vessel_name, filename);
        },
        R"pbdoc(Writes a PolarNode, PolarSet, Polar or PolarTable to a netCDF file)pbdoc",
        "polar_node"_a, "vessel_name"_a, "filename"_a);

  // ===================================================================================================================
  // Checker
  // ===================================================================================================================

  m.def("spec_check", [](const std::string &filename) -> bool {
          auto version = poem::get_version(filename);
          return poem::spec_check(filename, version);
        },
        R"pbdoc(Returns true if the file follows the specification number found in the file)pbdoc",
        "filename"_a
  );

  // ===================================================================================================================
  // Reader
  // ===================================================================================================================

  m.def("load", &poem::load,
        R"pbdoc(Writes a PolarNode, PolarSet, Polar or PolarTable to a netCDF file)pbdoc",
        "filename"_a, "vessel_name"_a, "spec_checking"_a = true);

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
