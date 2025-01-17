//
// Created by frongere on 17/01/25.
//

#ifndef POEM_IO_H
#define POEM_IO_H

#include <memory>
#include <netcdf>
#include <filesystem>
#include <semver/semver.hpp>

#include "exceptions.h"
#include "PolarTable.h"
#include "specifications/spec_checkers.h"

namespace fs = std::filesystem;

namespace poem {

  // ===================================================================================================================
  // I/O for PolarTable
  // ===================================================================================================================

  namespace internal {
    template<typename T>
    void write_polar_table_(netCDF::NcGroup &group, const netCDF::NcType &nc_type,
                            std::shared_ptr<PolarTable<T>> polar_table) {

      auto dimension_grid = polar_table->dimension_grid();
      auto dimension_set = dimension_grid->dimension_set();

      // Storing dimensions
      size_t ndim = polar_table->dim();
      std::vector<netCDF::NcDim> dims;
      dims.reserve(ndim);

      for (size_t i = 0; i < ndim; ++i) {
        auto dimension = dimension_set->dimension(i);
        auto name = dimension->name();
        auto values = dimension_grid->values(i);

        auto dim = group.getDim(name);
        if (dim.isNull()) {
          dim = group.addDim(name, values.size());

          // The dimension as a variable
          netCDF::NcVar nc_var = group.addVar(name, netCDF::ncDouble, dim);
          nc_var.setCompression(true, true, 5);
          nc_var.putVar(values.data());

          nc_var.putAtt("unit", dimension->unit());
          nc_var.putAtt("description", dimension->description());
        }

        dims.push_back(dim);

      }

      // Storing the values
      auto polar_name = polar_table->name();
      netCDF::NcVar nc_var = group.getVar(polar_name);

      if (nc_var.isNull()) {

        nc_var = group.addVar(polar_name, nc_type, dims);

//        switch (polar_table->type()) {
//          case POEM_DOUBLE:
//            nc_var = group.addVar(polar_name, netCDF::ncDouble, dims);
//            break;
//          case POEM_INT:
//            nc_var = group.addVar(polar_name, netCDF::ncInt, dims);
//
//        }
        nc_var.setCompression(true, true, 5);

        nc_var.putVar(polar_table->values().data());
        nc_var.putAtt("unit", polar_table->unit());
        nc_var.putAtt("description", polar_table->description());

      } else {
        spdlog::critical("Attempting to store more than one time a variable with the same name {}", polar_name);
        CRITICAL_ERROR_POEM
      }
    }
  }

  /**
   * Writes a PolarTableBase into a NetCDF group (non templated)
   */
  void write_polar_table(netCDF::NcGroup &group, std::shared_ptr<PolarTableBase> polar_table) {
    switch (polar_table->type()) {
      case POEM_DOUBLE:
        internal::write_polar_table_(group, netCDF::ncDouble,
                                     std::dynamic_pointer_cast<PolarTable<double>>(polar_table));
        break;
      case POEM_INT:
        internal::write_polar_table_(group, netCDF::ncInt, std::dynamic_pointer_cast<PolarTable<int>>(polar_table));
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

  /**
   * Read a NetCDF variable and returns it as a PolarTableBase
   *
   * @param var the NetCDF variable
   * @param dimension_grid reference to the DimensionGrid to be used
   * @param dimension_grid_from_var if true, the DimensionGrid is built from the variable,
   *                                otherwise, the one given as argument is used (must be valid)
   */
  std::shared_ptr<PolarTableBase>
  read_polar_table(const netCDF::NcVar &var, std::shared_ptr<DimensionGrid> &dimension_grid,
                   bool dimension_grid_from_var) {

    if (dimension_grid_from_var) {
      dimension_grid = read_dimension_grid_from_var(var);
    }

    if (var.getDimCount() != dimension_grid->dim()) {
      spdlog::critical("Dimension mismatch between netCDF dataset var and DimensionGrid");
      CRITICAL_ERROR_POEM
    }

    size_t ndim = dimension_grid->dim();
    size_t size = 0;

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

  // ===================================================================================================================
  // I/O for Polar
  // ===================================================================================================================

  void write_polar(netCDF::NcGroup &group, std::shared_ptr<Polar> polar) {
    std::shared_ptr<DimensionGrid> dimension_grid;
    for (const auto &polar_table: *polar) {
      write_polar_table(group, polar_table.second);
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
        std::string polar_mode_str;
        group.getAtt("POLAR_MODE").getValues(polar_mode_str);
        polar_mode = string_to_polar_mode(polar_mode_str);
        // TODO: verifier

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
      if (group.getAtts().contains("POLAR_MODE")) {
        std::string polar_mode_str;
        group.getAtt("POLAR_MODE").getValues(polar_mode_str);
        polar_mode = string_to_polar_mode(polar_mode_str);

      } else {
        spdlog::critical("Cannot infer the POLAR_MODE of group {}", polar_name);
        CRITICAL_ERROR_POEM
      }
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

      polar->add_polar_table(polar_table);
    }

    return polar;
  }

  // ===================================================================================================================
  // I/O for PolarSet
  // ===================================================================================================================

  void write_polar_set(netCDF::NcGroup &group, std::shared_ptr<PolarSet> polar_set) {
    for (const auto &polar: *polar_set) {
      auto new_group = group.addGroup(polar_mode_to_string(polar.first));
      write_polar(new_group, polar.second);
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
      polar_set->add_polar(polar);

    }

    return polar_set;
  }

  // ===================================================================================================================
  // I/O for OperationMode
  // ===================================================================================================================

  void write_operation_mode(netCDF::NcGroup &group, std::shared_ptr<OperationMode> operation_mode) {

    if (operation_mode->is_leaf()) {
      if (!operation_mode->has_polar_set()) {
        spdlog::critical("OperationMode {} is a leaf but has no PolarSet enclosed");
        CRITICAL_ERROR_POEM
      }
      write_polar_set(group, operation_mode->polar_set());
    } else {
      // just creating a subgroup and keep writing childs
      for (const auto &operation_mode_: operation_mode->children<OperationMode>()) {
        auto new_group = group.addGroup(operation_mode_->name());
        write_operation_mode(new_group, operation_mode_);
      }

    }
  }

  bool is_leaf_operation_mode(const netCDF::NcGroup &group) {
    auto subgroups = group.getGroups();
    return subgroups.contains("MPPP") ||
           subgroups.contains("HPPP") ||
           subgroups.contains("MVPP") ||
           subgroups.contains("HVPP") ||
           subgroups.contains("VPP");
  }

  std::string get_version_from_nc_file(const std::string& filename) {
    if (!fs::exists(filename)){
      spdlog::critical("NetCDF file not found: {}", filename);
      CRITICAL_ERROR_POEM
    }

    netCDF::NcFile datafile(std::string(filename), netCDF::NcFile::read);

    std::string poem_file_format_version;
    if(!datafile.getAtts().contains("poem_file_format_version")) {
      // This is likely to be a file with version v0
      poem_file_format_version = "v0";
    } else {
      std::string spec_version;
      datafile.getAtt("poem_file_format_version");
    }

    datafile.close();
    return poem_file_format_version;
  }

  std::shared_ptr<OperationMode> read_v0(const std::string &filename) {

    // OperationMode is root and we set the vessel name as filename as we do not have vessel name in v0
    std::string vessel_name = fs::path(filename).stem();

    auto operation_mode = std::make_shared<OperationMode>(vessel_name);
    auto polar_set = make_polar_set(vessel_name);
    operation_mode->set_polar_set(polar_set);

    netCDF::NcFile datafile(std::string(filename), netCDF::NcFile::read);
    auto polar = read_polar(datafile);
    datafile.close();

    polar_set->add_polar(polar);

    return operation_mode;
  }

  std::shared_ptr<OperationMode> read_v1(const std::string &filename) {

    auto operation_mode = std::make_shared<OperationMode>("");
    NIY_POEM
    return operation_mode;
  }

  std::shared_ptr<OperationMode> read_poem_nc_file(const std::string &filename) {

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

    std::shared_ptr<OperationMode> operation_mode;
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


//  void
//  read_operation_mode(const netCDF::NcGroup &group, std::shared_ptr<OperationMode> operation_mode) {
//    if (!group.isRootGroup()) {
//      NIY_POEM
//    }
//
//    NIY_POEM
//  }

//  std::shared_ptr<OperationMode> read_operation_mode(const netCDF::NcGroup &group) {
//
//    std::shared_ptr<OperationMode> operation_mode;
//    if (group.isRootGroup()) {
////      operation_mode = std::
//    }
//
//    std::shared_ptr<OperationMode> operation_mode;
//
//    if (is_leaf_operation_mode(group)) {
//      // Reading a PolarSet
//    } else {
//
//    }
//
//  }


}  // poem

#endif //POEM_IO_H
