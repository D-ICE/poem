//
// Created by frongere on 17/01/25.
//

#ifndef POEM_IO_H
#define POEM_IO_H

#include <memory>
#include <netcdf>

#include "exceptions.h"
#include "PolarTable.h"

namespace poem {

  // ===================================================================================================================
  // I/O for PolarTable
  // ===================================================================================================================

  namespace internal {
    template<typename T>
    void write_(netCDF::NcGroup &group, const netCDF::NcType &nc_type, std::shared_ptr<PolarTable<T>> polar_table) {

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
  void write(netCDF::NcGroup &group, std::shared_ptr<PolarTableBase> polar_table) {
    switch (polar_table->type()) {
      case POEM_DOUBLE:
        internal::write_(group, netCDF::ncDouble, std::dynamic_pointer_cast<PolarTable<double>>(polar_table));
        break;
      case POEM_INT:
        internal::write_(group, netCDF::ncInt, std::dynamic_pointer_cast<PolarTable<int>>(polar_table));
    }
  }

  std::shared_ptr<DimensionGrid> read_dimension_grid(const netCDF::NcVar &var) {

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
  read(const netCDF::NcVar &var, std::shared_ptr<DimensionGrid> &dimension_grid, bool dimension_grid_from_var) {

    if (dimension_grid_from_var) {
      dimension_grid = read_dimension_grid(var);
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

  void write(netCDF::NcGroup &group, std::shared_ptr<Polar> polar) {
    std::shared_ptr<DimensionGrid> dimension_grid;
    for (const auto &polar_table: *polar) {
      write(group, polar_table.second);
    }
    group.putAtt("POLAR_MODE", polar_mode_to_string(polar->mode()));
  }

  std::shared_ptr<Polar>
  read(const netCDF::NcGroup &group) {

    auto polar_name = group.getName(); // TODO: en prevision d'avoir un path, est-ce qu'on ne prend que le dernier element du path ?

    if (!is_polar_mode(polar_name)) {
      if (group.getAtts().contains("POLAR_MODE")) {
        group.getAtt("POLAR_MODE").getValues(polar_name);
        // TODO: ici la mecanique pour inferer le mode des polaires v0...
      } else {
        spdlog::critical("Cannot infer the POLAR_MODE of group {}", polar_name);
        CRITICAL_ERROR_POEM
      }
    }

    std::shared_ptr<DimensionGrid> dimension_grid;
    auto polar = make_polar(polar_name, string_to_polar_mode(polar_name), dimension_grid);

    bool dimension_grid_from_var = true;
    for (const auto &nc_var: group.getVars()) {
      if (group.getCoordVars().contains(nc_var.first)) continue;
      auto polar_table = read(nc_var.second, dimension_grid, dimension_grid_from_var);

      if (dimension_grid_from_var) {
        polar->dimension_grid() = dimension_grid;
        dimension_grid_from_var = false;
      }

      polar->add_polar_table(polar_table);
    }

    return polar;
  }

}  // poem

#endif //POEM_IO_H
