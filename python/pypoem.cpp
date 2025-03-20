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
  m.def("poem_lib_version", &poem::git::version_full,
        R"pbdoc(Get the POEM Library version)pbdoc");
  m.def("poem_lib_last_commit_date", &poem::git::last_commit_date,
        R"pbdoc(Get the POEM Library last revision date)pbdoc");
  m.def("poem_library_status", &poem::git::development_status,
        R"pbdoc(Get the development status POEM Library)pbdoc");

  m.def("mandatory_polar_tables",
        [](const std::string &polar_mode) -> std::unordered_map<std::string, std::vector<std::string>> {
          return poem::mandatory_polar_tables(poem::string_to_polar_mode(polar_mode),
                                              poem::current_poem_standard_version());
        },
        R"pbdoc(Get mandatory PolarTables for the current POEM Specification)pbdoc");

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
  DimensionGrid.def("ndims", &poem::DimensionGrid::ndims,
                    R"pbdoc("Get the number of Dimensions in the DimensionGrid")pbdoc");
  DimensionGrid.def("dimensions", [](const poem::DimensionGrid &self) -> std::vector<std::string> {
                      std::vector<std::string> dimensions;
                      dimensions.reserve(self.ndims());
                      for (const auto &dimension: *self.dimension_set()) {
                        dimensions.push_back(dimension->name());
                      }
                      return dimensions;
                    },
                    R"pbdoc("Get a list of Dimensions")pbdoc");
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
  DimensionGrid.def("size",
                    py::overload_cast<>(&poem::DimensionGrid::size, py::const_),
                    R"pbdoc("Get the size of the DimensionGrid")pbdoc");
  DimensionGrid.def("shape", &poem::DimensionGrid::shape,
                    R"pbdoc(Returns an ordered list of Dimension Sizes)pbdoc");

  m.def("make_dimension_grid", &poem::make_dimension_grid,
        R"pbdoc("Build a DimensionGrid)pbdoc");

  // ===================================================================================================================
  // DimensionPoint
  // ===================================================================================================================
  py::class_<poem::DimensionPoint> DimensionPoint(m, "DimensionPoint");
  DimensionPoint.doc() = R"pbdoc("A DimensionPoint is a point inside a DimensinoGrid with cartesian coordinates")pbdoc";
  DimensionPoint.def(py::init<std::shared_ptr<poem::DimensionSet>>());
  DimensionPoint.def("get",
                     py::overload_cast<const std::string &>(&poem::DimensionPoint::get),
                     R"pbdoc("Get the component value of the DimensionPoint from named Dimension")pbdoc");

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

  PolarNode.def(py::init<const std::string &, const std::string &>());

  PolarNode.def("name", &poem::PolarNode::name,
                R"pbdoc(Get the name of the PolarNode)pbdoc");
  PolarNode.def("description", &poem::PolarNode::description,
                R"pbdoc(Get the description of the PolarNode)pbdoc");
  PolarNode.def("full_name", [](const poem::PolarNode &self) -> std::string {
                  return self.full_name();
                },
                R"pbdoc(Get the full name of the PolarNode)pbdoc");

  PolarNode.def("child", &poem::PolarNode::child<poem::PolarNode>,
                R"pbdoc(Returns the child from name)pbdoc",
                "name"_a);
  PolarNode.def("children", &poem::PolarNode::children<poem::PolarNode>,
                R"pbdoc(Returns a list of children of current PolarNode)pbdoc");

  PolarNode.def("is_polar_node", [](const poem::PolarNode &self) -> bool {
                  return self.polar_node_type() == poem::POLAR_NODE;
                },
                R"pbdoc(Returns True if the PolarNode is a PolarNode)pbdoc");
  PolarNode.def("is_polar_set", [](const poem::PolarNode &self) -> bool {
                  return self.polar_node_type() == poem::POLAR_SET;
                },
                R"pbdoc(Returns True if the PolarNode is a PolarSet)pbdoc");
  PolarNode.def("is_polar", [](const poem::PolarNode &self) -> bool {
                  return self.polar_node_type() == poem::POLAR;
                },
                R"pbdoc(Returns True if the PolarNode is a PolarSet)pbdoc");
  PolarNode.def("is_polar_table", [](poem::PolarNode &self) -> bool {
                  return self.polar_node_type() == poem::POLAR_TABLE;
                },
                R"pbdoc(Returns True if the PolarNode is a PolarTable)pbdoc");
  PolarNode.def("is_polar_table_double", [](poem::PolarNode &self) -> bool {
                  return self.polar_node_type() == poem::POLAR_TABLE
                         && (self.as_polar_table()->type() == poem::POEM_DOUBLE);
                },
                R"pbdoc(Returns True if the PolarNode is a PolarTableDouble)pbdoc");
  PolarNode.def("is_polar_table_int", [](poem::PolarNode &self) -> bool {
                  return self.polar_node_type() == poem::POLAR_TABLE
                         && self.as_polar_table()->type() == poem::POEM_INT;
                },
                R"pbdoc(Returns True if the PolarNode is a PolarTableInt)pbdoc");

  PolarNode.def("as_polar_set", &poem::PolarNode::as_polar_set, R"pbdoc(Returns the associated PolarSet)pbdoc");
  PolarNode.def("as_polar", &poem::PolarNode::as_polar, R"pbdoc(Returns the associated Polar)pbdoc");
  PolarNode.def("as_polar_table_double", [](std::shared_ptr<poem::PolarNode> &self)
                    -> std::shared_ptr<poem::PolarTable<double>> {
                  return self->as_polar_table()->as_polar_table_double();
                },
                R"pbdoc(Returns the associated PolarTableDouble)pbdoc");
  PolarNode.def("as_polar_table_int", [](std::shared_ptr<poem::PolarNode> &self)
                    -> std::shared_ptr<poem::PolarTable<int>> {
                  return self->as_polar_table()->as_polar_table_int();
                },
                R"pbdoc(Returns the associated PolarTableInt)pbdoc");
  PolarNode.def("attach_polar_node", &poem::PolarNode::attach_polar_node,
                R"pbdoc("attach a PolarNode to this PolarNode")pbdoc",
                "polar_node"_a
                );
  PolarNode.def("attach_polar_set", &poem::PolarNode::attach_polar_set,
                R"pbdoc("attach a PolarSet to this PolarNode")pbdoc",
                "polar_set"_a
                );

  PolarNode.def("exists", [](poem::PolarNode &self, const std::string &path) -> bool {
                  return self.exists(path);
                },
                R"pbdoc(Returns True if the path exists)pbdoc");
  PolarNode.def("polar_node_from_path", [](poem::PolarNode &self, const std::string &path)
                    -> std::shared_ptr<poem::PolarNode> {
                  return self.polar_node_from_path(path);
                },
                R"pbdoc(Get a PolarNode from path)pbdoc");

  PolarNode.def("layout", [](const poem::PolarNode &self, const int indent) -> std::string {
                  json json_node = self.layout();
                  return json_node.dump(indent);
                },
                R"pbdoc(Returns a json string as a layout for the tree starting at current PolarNode)pbdoc",
                "indent"_a = -1);

  m.def("make_polar_node", &poem::make_polar_node,
        R"pbdoc("Build a PolarNode")pbdoc",
        "name"_a, "description"_a);

  // ===================================================================================================================
  // PolarTable<T>
  // ===================================================================================================================

  // -------------------------------------------- PolarTableDouble -----------------------------------------------------
  py::class_<poem::PolarTable<double>, std::shared_ptr<poem::PolarTable<double>>, poem::PolarNode>
      PolarTableDouble(m, "PolarTableDouble");
  PolarTableDouble.doc() = R"pbdoc("A PolarTableDouble is a special PolarNode used to represent a specific variable
                                    stored in a multidimensional array. Double version.")pbdoc";
  PolarTableDouble.def("name", &poem::PolarTable<double>::name,
                       R"pbdoc(Get the name of the PolarTableDouble)pbdoc");
  PolarTableDouble.def("unit", &poem::PolarTable<double>::unit,
                       R"pbdoc(Get the unit of the PolarTableDouble)pbdoc");
  PolarTableDouble.def("description", &poem::PolarTable<double>::description,
                       R"pbdoc(Get the description of the PolarTableDouble)pbdoc");
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
  PolarTableDouble.def("array",
                       [](poem::PolarTable<double> &self) -> py::array_t<double> {
                         return vector2memoryview(self.values(), self.dimension_grid()->shape());
                       },
                       R"pbdoc(Returns the PolarTable NDArray (no copy))pbdoc");
  PolarTableDouble.def("copy", &poem::PolarTable<double>::copy,
                       R"pbdoc(Get a copy of the PolarTableDouble)pbdoc");
  PolarTableDouble.def("dimension_grid", &poem::PolarTable<double>::dimension_grid,
                       R"pbdoc(Returns the DimensionGrid associated to the PolarTable)pbdoc");
  PolarTableDouble.def("slice", [](const poem::PolarTable<double> &self,
                                   const std::unordered_map<std::string, double> &prescribed_values,
                                   const std::string &oob_method)
                           -> std::shared_ptr<poem::PolarTable<double>> {
                         return self.slice(prescribed_values, poem::string_to_outofbound_method(oob_method));
                       },
                       R"pbdoc(Returns a sliced PolarTableDouble)pbdoc",
                       "prescribed_values"_a, "oob_method"_a = "error");
  PolarTableDouble.def("squeeze", py::overload_cast<>(&poem::PolarTable<double>::squeeze),
                       R"pbdoc(Removes singleton dimensions from PolarTableInt (inplace))pbdoc");
  PolarTableDouble.def("nearest", [](const poem::PolarTable<double> &self,
                                     const std::unordered_map<std::string, double> &point_dict,
                                     const std::string &oob_method) -> double {
                         // TODO: tester les bornes des valeurs
                         if (point_dict.size() != self.dim()) {
                           LogCriticalError("In PolarTableDouble {} of dimension {}, "
                                            "nearest function called with incorrect number of values {}",
                                            self.name(), self.dim(), point_dict.size());
                           CRITICAL_ERROR_POEM
                         }

                         auto dimension_set = self.dimension_grid()->dimension_set();
                         std::vector<double> array(self.dim());
                         size_t i = 0;
                         for (const auto &dimension: *dimension_set) {
                           array[i] = point_dict.at(dimension->name());
                           i++;
                         }
                         return self.nearest(poem::DimensionPoint(dimension_set, array),
                                             poem::string_to_outofbound_method(oob_method));
                       },
                       R"pbdoc("Get the nearest value for the values given as a dictionary")pbdoc",
                       "point_dict"_a, "oob_method"_a = "error");
  PolarTableDouble.def("interp", [](const poem::PolarTable<double> &self,
                                    const std::unordered_map<std::string, double> &point_dict,
                                    const std::string &oob_method) -> double {
                         // TODO: tester les bornes des valeurs
                         if (point_dict.size() != self.dim()) {
                           LogCriticalError("In PolarTableDouble {} of dimension {}, "
                                            "interp function called with incorrect number of values {}",
                                            self.name(), self.dim(), point_dict.size());
                           CRITICAL_ERROR_POEM
                         }

                         auto dimension_set = self.dimension_grid()->dimension_set();
                         std::vector<double> array(self.dim());
                         size_t i = 0;
                         for (const auto &dimension: *dimension_set) {
                           array[i] = point_dict.at(dimension->name());
                           i++;
                         }
                         return self.interp(poem::DimensionPoint(dimension_set, array),
                                            poem::string_to_outofbound_method(oob_method));
                       },
                       R"pbdoc("Get an interpolated value at point_dict")pbdoc",
                       "point_dict"_a, "oob_method"_a = "error");

  m.def("make_polar_table_double", &poem::make_polar_table_double,
        R"pbdoc("Build a PolarTable containing double values")pbdoc",
        "name"_a, "unit"_a, "description"_a, "dimension_grid"_a);

  // -------------------------------------------- PolarTableInt -----------------------------------------------------
  py::class_<poem::PolarTable<int>, std::shared_ptr<poem::PolarTable<int>>, poem::PolarNode>
      PolarTableInt(m, "PolarTableInt");
  PolarTableInt.doc() = R"pbdoc("A PolarTableInt is a special PolarNode used to represent a specific variable
                                 stored in a multidimensional array. Int version.")pbdoc";
  PolarTableInt.def("name", &poem::PolarTable<int>::name,
                    R"pbdoc(Get the name of the PolarTableInt)pbdoc");
  PolarTableInt.def("unit", &poem::PolarTable<int>::unit,
                    R"pbdoc(Get the unit of the PolarTableInt)pbdoc");
  PolarTableInt.def("description", &poem::PolarTable<int>::description,
                    R"pbdoc(Get the description of the PolarTableInt)pbdoc");
  PolarTableInt.def("fill_with", &poem::PolarTable<int>::fill_with,
                    R"pbdoc()pbdoc", "value"_a);
  PolarTableInt.def("set_values",
                    [](poem::PolarTable<int> &self,
                       const py::array_t<int, py::array::c_style | py::array::forcecast> &array) -> void {
                      self.set_values(ndarray2stdvector<int>(array));
                    },
                    R"pbdoc()pbdoc");
  PolarTableInt.def("set_value",
                    py::overload_cast<std::vector<size_t>, const int &>(&poem::PolarTable<int>::set_value));
  PolarTableInt.def("array",
                    [](poem::PolarTable<int> &self) -> py::array_t<int> {
                      return vector2memoryview(self.values(), self.dimension_grid()->shape());
                    },
                    R"pbdoc(Returns the PolarTable NDArray (no copy))pbdoc");
  PolarTableInt.def("copy", &poem::PolarTable<int>::copy,
                    R"pbdoc(Get a copy of the PolarTableInt)pbdoc");
  PolarTableInt.def("dimension_grid", &poem::PolarTable<int>::dimension_grid,
                    R"pbdoc(Returns the DimensionGrid associated to the PolarTable)pbdoc");
  PolarTableInt.def("slice", [](const poem::PolarTable<int> &self,
                                const std::unordered_map<std::string, double> &prescribed_values,
                                const std::string &oob_method)
                        -> std::shared_ptr<poem::PolarTable<int>> {
                      return self.slice(prescribed_values, poem::string_to_outofbound_method(oob_method));
                    },
                    R"pbdoc(Returns a sliced PolarTableInt)pbdoc",
                    "prescribed_values"_a, "oob_method"_a = "error");
  PolarTableInt.def("squeeze", py::overload_cast<>(&poem::PolarTable<int>::squeeze),
                    R"pbdoc(Removes singleton dimensions from PolarTableInt (inplace))pbdoc");
  PolarTableInt.def("nearest", [](const poem::PolarTable<int> &self,
                                  const std::unordered_map<std::string, double> &point_dict,
                                  const std::string &oob_method) -> int {
                      // TODO: tester les bornes des valeurs
                      if (point_dict.size() != self.dim()) {
                        LogCriticalError("In PolarTableDouble {} of dimension {}, "
                                         "nearest function called with incorrect number of values {}",
                                         self.name(), self.dim(), point_dict.size());
                        CRITICAL_ERROR_POEM
                      }

                      auto dimension_set = self.dimension_grid()->dimension_set();
                      std::vector<double> array(self.dim());
                      size_t i = 0;
                      for (const auto &dimension: *dimension_set) {
                        array[i] = point_dict.at(dimension->name());
                        i++;
                      }
                      return self.nearest(poem::DimensionPoint(dimension_set, array),
                                          poem::string_to_outofbound_method(oob_method));
                    },
                    R"pbdoc("Get the nearest value for the values given as a dictionary")pbdoc",
                    "point_dict"_a, "oob_method"_a = "error");
  PolarTableInt.def("interp", [](const poem::PolarTable<int> &self,
                                 const std::unordered_map<std::string, double> &point_dict,
                                 const std::string &oob_method) -> int {
                      // TODO: tester les bornes des valeurs
                      if (point_dict.size() != self.dim()) {
                        LogCriticalError("In PolarTableDouble {} of dimension {}, "
                                         "interp function called with incorrect number of values {}",
                                         self.name(), self.dim(), point_dict.size());
                        CRITICAL_ERROR_POEM
                      }

                      auto dimension_set = self.dimension_grid()->dimension_set();
                      std::vector<double> array(self.dim());
                      size_t i = 0;
                      for (const auto &dimension: *dimension_set) {
                        array[i] = point_dict.at(dimension->name());
                        i++;
                      }
                      return self.interp(poem::DimensionPoint(dimension_set, array),
                                         poem::string_to_outofbound_method(oob_method));
                    },
                    R"pbdoc("Get an interpolated value at point_dict")pbdoc",
                    "point_dict"_a, "oob_method"_a = "error");


  m.def("make_polar_table_int", &poem::make_polar_table_int,
        R"pbdoc(Build a PolarTable containing int values)pbdoc"
        "name"_a, "unit"_a, "description"_a, "dimension_grid"_a);


  // ===================================================================================================================
  // Polar
  // ===================================================================================================================
  py::class_<poem::Polar, std::shared_ptr<poem::Polar>, poem::PolarNode> Polar(m, "Polar");
  Polar.doc() = R"pbdoc("A Polar is a special PolarNode used to group PolarTables relative to a specific POLAR_MODE")pbdoc";
  Polar.def(py::init<const std::string &, poem::POLAR_MODE, std::shared_ptr<poem::DimensionGrid>>());
  Polar.def("name", &poem::Polar::description,
            R"pbdoc(Get the name of the Polar)pbdoc");
  Polar.def("description", &poem::Polar::description,
            R"pbdoc(Get the description of the Polar)pbdoc");
  Polar.def("mode", [](const poem::Polar &self) -> std::string {
              return poem::polar_mode_to_string(self.mode());
            },
            R"pbdoc(Get the mode of the Polar)pbdoc");
  Polar.def("dimension_grid", [](const poem::Polar &self) -> std::shared_ptr<poem::DimensionGrid> {
              return self.dimension_grid();
            },
            R"pbdoc(Get the DimensionGrid of the Polar)pbdoc");
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
  Polar.def("remove_polar_table", &poem::Polar::remove_polar_table,
            R"pbdoc("Remove a PolarTable for the Polar")pbdoc",
            "name"_a);

  m.def("make_polar", &poem::make_polar,
        R"pbdoc("Make a Polar")pbdoc",
        "name"_a, "polar_mode"_a, "dimension_grid"_a);

  // ===================================================================================================================
  // PolarSet
  // ===================================================================================================================
  py::class_<poem::PolarSet, std::shared_ptr<poem::PolarSet>, poem::PolarNode> PolarSet(m, "PolarSet");
  PolarSet.doc() = R"pbdoc("A PolarSet is a special PolarNode used to group a set of Polar")pbdoc";
  PolarSet.def(py::init<const std::string &, const std::string &>());
  PolarSet.def("name", &poem::PolarSet::description,
               R"pbdoc(Get the name of the PolarSet)pbdoc");
  PolarSet.def("description", &poem::PolarSet::description,
               R"pbdoc(Get the description of the PolarSet)pbdoc");
  PolarSet.def("attach_polar", &poem::PolarSet::attach_polar,
               R"pbdoc("attach a Polar to this PolarSet")pbdoc",
               "polar"_a);

  m.def("make_polar_set", &poem::make_polar_set,
        R"pbdoc("Build a PolarSet")pbdoc",
        "name"_a, "description"_a);

  // ===================================================================================================================
  // Writer
  // ===================================================================================================================
  m.def("to_netcdf", [](std::shared_ptr<poem::PolarNode> polar_node,
                        const std::string &vessel_name,
                        const std::string &filename,
                        bool verbose) -> void {
          poem::to_netcdf(polar_node, vessel_name, filename, verbose);
        },
        R"pbdoc(Writes a PolarNode, PolarSet, Polar or PolarTable to a netCDF file)pbdoc",
        "polar_node"_a, "vessel_name"_a, "filename"_a, "verbose"_a = true);

  // ===================================================================================================================
  // Checker
  // ===================================================================================================================
  m.def("get_version", &poem::get_version,
        R"pbdoc(Get the version of a POEM File)pbdoc",
        "filename"_a);

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
        "filename"_a, "spec_checking"_a = true, "verbose"_a = true);

}  // PYBIND11_MODULE(pypoem, m)
