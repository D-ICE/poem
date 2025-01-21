//
// Created by frongere on 21/01/25.
//

#include "IO.h"

#include <semver/semver.hpp>

#include "enums.h"
#include "PolarTable.h"
#include "Polar.h"
#include "PolarSet.h"
#include "specifications/spec_checkers.h"

namespace poem {

  void write_polar_table(std::shared_ptr<PolarTableBase> polar_table, netCDF::NcGroup &group) {
    switch (polar_table->type()) {
      case POEM_DOUBLE:
        internal::write_polar_table_(polar_table->as_polar_table_double(), netCDF::ncDouble, group);

        break;
      case POEM_INT:
        internal::write_polar_table_(polar_table->as_polar_table_int(), netCDF::ncInt, group);
    }
  }

  std::shared_ptr<DimensionGrid> read_dimension_grid_from_var(const netCDF::NcVar &var) {

    size_t ndim = var.getDimCount();

    auto group = var.getParentGroup();

    std::vector<std::shared_ptr<Dimension>> dimensions(ndim);
    for (size_t idim = 0; idim < ndim; ++idim) {
      auto dim = var.getDim((int) idim);
      std::string name = dim.getName();
      auto dim_var = group.getVar(name);
      std::string unit, description;
      dim_var.getAtt("unit").getValues(unit);
      dim_var.getAtt("description").getValues(description);

      dimensions[idim] = std::make_shared<Dimension>(name, unit, description);
    }

    auto dimension_set = make_dimension_set(dimensions);
    auto dimension_grid = make_dimension_grid(dimension_set);

    for (size_t idim = 0; idim < ndim; ++idim) {
      auto name = dimension_set->dimension(idim)->name();
      auto dim_var = group.getVar(name);
      std::vector<double> values(dim_var.getDim(0).getSize());
      dim_var.getVar(values.data());

      dimension_grid->set_values(name, values);

    }

    return dimension_grid;
  }

  std::shared_ptr<PolarTableBase>
  read_polar_table(const netCDF::NcVar &var,
                   std::shared_ptr<DimensionGrid> &dimension_grid,
                   bool dimension_grid_from_var) {

    if (dimension_grid_from_var) {
      dimension_grid = read_dimension_grid_from_var(var);
    }

    if (var.getDimCount() != dimension_grid->dim()) {
      spdlog::critical("Dimension mismatch between netCDF dataset var and DimensionGrid");
      CRITICAL_ERROR_POEM
    }

    size_t ndim = dimension_grid->dim();

    std::vector<std::shared_ptr<Dimension>> dimensions(ndim);
    for (size_t idim = 0; idim < ndim; ++idim) {
      if (var.getDim((int) idim).getName() != dimension_grid->dimension_set()->dimension(idim)->name()) {
        spdlog::critical("Dimension name mismatch between netCDF dataset and DimensionGrid");
        CRITICAL_ERROR_POEM
      }
    }

    std::string name = var.getName();
    std::string unit;
    var.getAtt("unit").getValues(unit);
    std::string description;
    var.getAtt("description").getValues(description);

    std::shared_ptr<PolarTableBase> polar_table;

    switch (var.getType().getTypeClass()) {
      case netCDF::NcType::nc_DOUBLE: {
        auto polar_table_ = make_polar_table<double>(name, unit, description, POEM_DOUBLE, dimension_grid);
        var.getVar(polar_table_->values().data());
        polar_table = polar_table_;
        break;
      }

      case netCDF::NcType::nc_INT: {
        auto polar_table_ = make_polar_table<int>(name, unit, description, POEM_INT, dimension_grid);
        var.getVar(polar_table_->values().data());
        polar_table = polar_table_;
        break;
      }

      default:
        spdlog::critical("Cannot read netCDF variable of type {}", var.getType().getName());
        CRITICAL_ERROR_POEM
    }

    return polar_table;

  }

  void write_polar(std::shared_ptr<Polar> polar, netCDF::NcGroup &group) {
    std::shared_ptr<DimensionGrid> dimension_grid;
    for (const auto &polar_table: polar->children<PolarTableBase>()) {
      write_polar_table(polar_table, group);
    }
    // FIXME: un peu premature la gestion des atteibuts...
    group.putAtt("POLAR_MODE", polar_mode_to_string(polar->mode()));
  }

  std::shared_ptr<Polar> read_polar(const netCDF::NcGroup &group) {

    auto polar_name = group.getName(); // TODO: en prevision d'avoir un path, est-ce qu'on ne prend que le dernier element du path ?

    POLAR_MODE polar_mode;
    if (!is_polar_mode(polar_name)) {
      // Trying to infer polar_mode from group name

      if (group.getAtts().contains("POLAR_MODE")) {
        group.getAtt("POLAR_MODE").getValues(polar_name);
        polar_mode = string_to_polar_mode(polar_name);

      } else if (group.getAtts().contains("vessel_type")) {
        // For v0...
        std::string vessel_type;
        group.getAtt("vessel_type").getValues(vessel_type);
        if (vessel_type == "HYBRID") {
          polar_mode = HPPP;
        } else if (vessel_type == "MOTOR") {
          polar_mode = MPPP;
        } else {
          spdlog::critical("While reading file with format v0, vessel_type {} unknown");
          CRITICAL_ERROR_POEM
        }

        polar_name = polar_mode_to_string(polar_mode);

      } else {
        spdlog::critical("Cannot infer the POLAR_MODE of group {}", polar_name);
        CRITICAL_ERROR_POEM
      }
    } else {
      polar_mode = string_to_polar_mode(polar_name);
    }

    std::shared_ptr<DimensionGrid> dimension_grid;
    auto polar = make_polar(polar_name, polar_mode, dimension_grid);

    bool dimension_grid_from_var = true;
    for (const auto &nc_var: group.getVars()) {
      if (group.getCoordVars().contains(nc_var.first)) continue;
      auto polar_table = read_polar_table(nc_var.second, dimension_grid, dimension_grid_from_var);

      if (dimension_grid_from_var) {
        polar->dimension_grid() = dimension_grid;
        dimension_grid_from_var = false;
      }

      polar->attach_polar_table(polar_table);
    }

    return polar;
  }

  void write_polar_set(std::shared_ptr<PolarSet> polar_set, netCDF::NcGroup &group) {
    for (const auto &polar: polar_set->children<Polar>()) {
      auto new_group = group.addGroup(polar_mode_to_string(polar->mode()));
      write_polar(polar, new_group);
    }
  }

  std::shared_ptr<PolarSet> read_polar_set(const netCDF::NcGroup &group) {

    std::string polar_set_name;
    if (group.isRootGroup()) {
      polar_set_name = "default";
    } else {
      polar_set_name = group.getName();
    }

    auto polar_set = std::make_shared<PolarSet>(polar_set_name);

    for (const auto &subgroup: group.getGroups()) {
      if (!is_polar_mode(subgroup.first)) continue; // Group name for Polar MUST be a POLAR_MODE string representation

      auto polar = read_polar(subgroup.second);

      polar_set->attach_polar(polar);

    }

    return polar_set;
  }

  void write_polar_node(std::shared_ptr<PolarNode> polar_node, netCDF::NcGroup &group) {
    // just creating a subgroup and keep writing children
    for (const auto &polar_node_: polar_node->children<PolarNode>()) {
      auto new_group = group.addGroup(polar_node_->name());
      to_netcdf(polar_node_, new_group);
    }
  }

//  bool is_leaf_operation_mode(const netCDF::NcGroup &group) {
//    auto subgroups = group.getGroups();
//    return subgroups.contains("MPPP") ||
//           subgroups.contains("HPPP") ||
//           subgroups.contains("MVPP") ||
//           subgroups.contains("HVPP") ||
//           subgroups.contains("VPP");
//  }

  std::string get_version_from_nc_file(const std::string &filename) {
    if (!fs::exists(filename)) {
      spdlog::critical("NetCDF file not found: {}", filename);
      CRITICAL_ERROR_POEM
    }

    netCDF::NcFile datafile(std::string(filename), netCDF::NcFile::read);

    std::string poem_file_format_version;
    if (!datafile.getAtts().contains("poem_file_format_version")) {
      // This is likely to be a file with version v0
      poem_file_format_version = "v0";
    } else {
      std::string spec_version;
      datafile.getAtt("poem_file_format_version");
    }

    datafile.close();
    return poem_file_format_version;
  }

  std::shared_ptr<PolarNode> read_v0(const std::string &filename) {

    // PolarNode is root and we set the vessel name as filename as we do not have vessel name in v0
    std::string vessel_name = fs::path(filename).stem();

    auto polar_set= std::make_shared<PolarSet>(vessel_name);

    netCDF::NcFile datafile(std::string(filename), netCDF::NcFile::read);
    auto polar = read_polar(datafile);
    datafile.close();

    polar_set->attach_polar(polar);

    return polar_set;
  }

  std::shared_ptr<PolarNode> read_v1(const std::string &filename) {

    auto operation_mode = std::make_shared<PolarNode>("");
    NIY_POEM
    return operation_mode;
  }

  std::shared_ptr<PolarNode> read_poem_nc_file(const std::string &filename) {

    spdlog::info("Reading file: {}", filename);

    auto poem_file_format_version = get_version_from_nc_file(filename);
    int major_version = (int) semver::version::parse(poem_file_format_version, false).major();

    spdlog::info("POEM file format version detected: v{}", major_version);
    if (!spec_check(filename, major_version)) {
      spdlog::critical("File is not compliant with version v{}", major_version);
      CRITICAL_ERROR_POEM
    } else {
      spdlog::info("File is compliant with version v{}", major_version);
    }

    std::shared_ptr<PolarNode> operation_mode;
    switch (major_version) {
      case 0:
        operation_mode = read_v0(filename);
        break;
      case 1:
        operation_mode = read_v1(filename);
        break;
    }

    return operation_mode;
  }


  void to_netcdf(std::shared_ptr<PolarNode> polar_node, netCDF::NcGroup &group) {

    switch (polar_node->polar_node_type()) {
      case POLAR_NODE: {
        write_polar_node(polar_node, group);
        break;
      }
      case POLAR_SET: {
        write_polar_set(polar_node->as_polar_set(), group);
        break;
      }
      case POLAR: {
        NIY_POEM
        break;
      }
      case POLAR_TABLE: {
        write_polar_table(polar_node->as_polar_table(), group);
        break;
      }
    }

  }

}  // poem
