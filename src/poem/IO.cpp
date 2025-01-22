//
// Created by frongere on 21/01/25.
//

#include "IO.h"

#include <semver/semver.hpp>

#include "PolarTable.h"
#include "Polar.h"
#include "PolarSet.h"
#include "specifications/spec_checkers.h"

namespace poem {

  // ===================================================================================================================
  // WRITERS
  // ===================================================================================================================

  int current_poem_standard_version() {
    return semver::version::parse(git::LastTag(), false).major();
  }

  std::vector<netCDF::NcDim> write_dimension_grid(std::shared_ptr<DimensionGrid> dimension_grid,
                                                  netCDF::NcGroup &group) {
    // Does the group has already these dimensions (and only these dimensions with the same data)

    std::vector<netCDF::NcDim> dims;
    dims.reserve(dimension_grid->ndims());
    if (group.getDims().empty()) {
      // No dimensions found in the group. Writing them

      for (const auto &dimension: *dimension_grid->dimension_set()) {
        // Adding dimensions to the group
        auto dim_name = dimension->name();
        auto dim = group.addDim(dim_name, dimension_grid->size(dim_name));
        dims.push_back(dim);

        // Write variables to the group
        auto nc_var = group.addVar(dim_name, netCDF::ncDouble, dim);
        nc_var.setCompression(true, true, 5);
        nc_var.putVar(dimension_grid->values(dim_name).data());

        nc_var.putAtt("unit", dimension->unit());
        nc_var.putAtt("description", dimension->description());
      }

    } else {
      // Checking that dimensions and CoordVars of the group are consistent with specified DimensionGrid
      for (const auto &dimension: *dimension_grid->dimension_set()) {
        // Does the group has this dimension?
        if (!group.getDims().contains(dimension->name())) {
          spdlog::critical("In group {}, dimension {} has no corresponding CoordVar",
                           group.getName(), dimension->name());
          CRITICAL_ERROR_POEM
        }
        // Does the group has the corresponding variable?
        if (!group.getDims().contains(dimension->name())) {
          spdlog::critical("Group {} does not have a CoordVar associated to dimension {}",
                           group.getName(), dimension->name());
        }
        // Does the dimension has the correct size?
        if (group.getDim(dimension->name()).getSize() != dimension_grid->size(dimension->name())) {
          spdlog::critical("Group {} dimension {} has inconsistent dimension size. Found {} expected {}",
                           group.getName(),
                           dimension->name(),
                           group.getDim(dimension->name()).getSize(),
                           dimension_grid->size(dimension->name()));
          CRITICAL_ERROR_POEM
        }
        // Does the variable has the correct values?
        auto nc_var = group.getVar(dimension->name());
        std::vector<double> values(dimension_grid->size(dimension->name()));
        nc_var.getVar(values.data());

        if (dimension_grid->values(dimension->name()) != values) {
          spdlog::critical("Coord Variable {} of group {} has inconsistent values with specified DimensionGrid",
                           nc_var.getName(), group.getName());
          CRITICAL_ERROR_POEM
        }

        // Same unit and description?
        std::string unit;
        nc_var.getAtt("unit").getValues(unit);
        if (unit != dimension->unit()) {
          spdlog::critical("Group {} dimension {} has not the same unit as the one of specified DimensionGrid",
                           group.getName(), dimension->name());
          CRITICAL_ERROR_POEM
        }

        std::string description;
        nc_var.getAtt("description").getValues(description);
        if (description != dimension->description()) {
          spdlog::critical("Group {} dimension {} has not the same description as the one of specified DimensionGrid",
                           group.getName(), dimension->name());
          CRITICAL_ERROR_POEM
        }

        dims.push_back(group.getDim(dimension->name()));
      }
    }

    return dims;
  }

  void to_netcdf(const Attributes &attributes, netCDF::NcGroup &group) {
    for (const auto &attribute: attributes) {
      group.putAtt(attribute.first, attribute.second);
    }
  }

  void to_netcdf(std::shared_ptr<Polar> polar, netCDF::NcGroup &group) {
    for (const auto &polar_table: polar->children<PolarTableBase>()) {
      to_netcdf(polar_table, group);
    }
    auto attributes = polar->attributes();
    if (!attributes.contains("polar_mode")) {
      group.putAtt("polar_mode", polar_mode_to_string(polar->mode()));
    }
    to_netcdf(attributes, group);
  }

  void to_netcdf(std::shared_ptr<PolarSet> polar_set, netCDF::NcGroup &group) {
    for (const auto &polar: polar_set->children<Polar>()) {
      auto new_group = group.addGroup(polar_mode_to_string(polar->mode()));
      to_netcdf(polar, new_group);
    }
    to_netcdf(polar_set->attributes(), group);
  }

  void to_netcdf(std::shared_ptr<PolarNode> polar_node, netCDF::NcGroup &group) {

    int major_version = current_poem_standard_version();

    switch (polar_node->polar_node_type()) {
      case POLAR_NODE: {
        for (const auto &polar_node_: polar_node->children<PolarNode>()) {
          auto new_group = group.addGroup(polar_node_->name());
          to_netcdf(polar_node_, new_group);
        }
        break;
      }
      case POLAR_SET: {
        to_netcdf(polar_node->as_polar_set(), group);
        break;
      }
      case POLAR: {
        to_netcdf(polar_node->as_polar(), group);
        break;
      }
      case POLAR_TABLE: {
        auto type = polar_node->as_polar_table()->type();
        switch (type) {
          case POEM_DOUBLE:
            to_netcdf(polar_node->as_polar_table_double(), netCDF::ncDouble, group);
            break;
          case POEM_INT:
            to_netcdf(polar_node->as_polar_table_int(), netCDF::ncInt, group);
            break;
        }
        break;
      }
    }

  }

  void to_netcdf(std::shared_ptr<PolarNode> polar_node, const std::string &filename) {
    spdlog::info("Writing file (version v{}): {}",
                 current_poem_standard_version(),
                 fs::absolute(filename).string());

    netCDF::NcFile root_group(filename, netCDF::NcFile::replace);
    to_netcdf(polar_node, root_group);
    root_group.close();
  }

  // ===================================================================================================================
  // READERS
  // ===================================================================================================================

  int get_version(const std::string &filename) {
    if (!fs::exists(filename)) {
      spdlog::critical("NetCDF file not found: {}", filename);
      CRITICAL_ERROR_POEM
    }

    netCDF::NcFile datafile(std::string(filename), netCDF::NcFile::read);

    int major_version;
    if (datafile.getAtts().contains("poem_file_format_version")) {
      std::string spec_version;
      datafile.getAtt("poem_file_format_version").getValues(spec_version);
      major_version = (int) semver::version::parse(spec_version, false).major();

    } else {
      // This is likely to be a file with version v0
      major_version = 0;
    }
    datafile.close();

    return major_version;
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

  std::shared_ptr<PolarTableBase> read_polar_table(const netCDF::NcVar &var,
                                                   std::shared_ptr<DimensionGrid> &dimension_grid,
                                                   bool dimension_grid_from_var) {

    if (dimension_grid_from_var) {
      dimension_grid = read_dimension_grid_from_var(var);
    }

    if (var.getDimCount() != dimension_grid->ndims()) {
      spdlog::critical("Dimension mismatch between netCDF dataset var and DimensionGrid");
      CRITICAL_ERROR_POEM
    }

    size_t ndim = dimension_grid->ndims();

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

  std::shared_ptr<Polar> read_polar(const netCDF::NcGroup &group, const std::string &mode) {

    auto polar_name = group.getName();

    POLAR_MODE polar_mode;
    if (!is_polar_mode(polar_name)) {
      // Trying to infer polar_mode from group name

      if (group.getAtts().contains("polar_mode")) {
        group.getAtt("polar_mode").getValues(polar_name);
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

      } else {
        spdlog::critical("Cannot infer the POLAR_MODE of group {} while reading Polar", polar_name);
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

  std::shared_ptr<PolarSet> read_polar_set(const netCDF::NcGroup &group, const std::string &polar_set_name) {

    std::string polar_set_name_;
    if (polar_set_name == "from_group") {
      if (group.isRootGroup()) {
        spdlog::critical("While reading PolarSet, if the given group is root, the name must be specified");
        CRITICAL_ERROR_POEM
      } else {
        polar_set_name_ = group.getName();
      }
    } else {
      polar_set_name_ = polar_set_name;
    }

    auto polar_set = std::make_shared<PolarSet>(polar_set_name_);

    for (const auto &subgroup: group.getGroups()) {
      if (!is_polar_mode(subgroup.first)) continue; // Group name for Polar MUST be a POLAR_MODE string representation

      auto polar = read_polar(subgroup.second);

      polar_set->attach_polar(polar);

    }

    return polar_set;
  }

  std::shared_ptr<PolarNode> read_polar_node(const netCDF::NcGroup &group, const std::string &polar_node_name_) {

    std::string polar_node_name;
    if (polar_node_name_ == "from_group") {
      if (group.isRootGroup()) {
        if (group.getAtts().contains("vessel_name")) {
          group.getAtt("vessel_name").getValues(polar_node_name);
        } else {
          spdlog::critical("Cannot infer name of the PolarNode");
          CRITICAL_ERROR_POEM
        }
      } else {
        polar_node_name = group.getName();
      }
    } else {
      polar_node_name = polar_node_name_;
    }

    std::shared_ptr<PolarNode> polar_node;

    auto polar_mode_type = guess_polar_node_type(group);
    switch (polar_mode_type) {
      case POLAR_NODE: {
        polar_node = make_polar_node(polar_node_name);
        for (const auto& subgroup : group.getGroups()) {
          auto new_polar_node = read_polar_node(subgroup.second);
          new_polar_node->attach_to(polar_node);
        }
        break;
      }
      case POLAR_SET: {
        polar_node = read_polar_set(group, polar_node_name);
        break;
      }
      case POLAR: {
        polar_node = read_polar(group, polar_node_name);
        break;
      }
      default:
        spdlog::critical("Something went wrong, we should never be here...");
        CRITICAL_ERROR_POEM
    }

    if (group.isRootGroup()) {
      polar_node->attributes().add_attribute("vessel_name", polar_node_name);
    }
    return polar_node;
  }


  std::shared_ptr<PolarNode> read_v0(const std::string &filename, const std::string &root_name) {

    // PolarNode is root and we set the vessel name as filename as we do not have vessel name in v0
//    std::string vessel_name = fs::path(filename).stem();
    if (root_name == "from_file") {
      spdlog::critical("While reading POEM file version v0, root_name must be specified");
      CRITICAL_ERROR_POEM
    }

    auto root_node = std::make_shared<PolarSet>(root_name);
    root_node->attributes().add_attribute("vessel_name", root_name);

    netCDF::NcFile datafile(filename, netCDF::NcFile::read);
    auto polar = read_polar(datafile);
    datafile.close();

    root_node->attach_polar(polar);

    return root_node;
  }

  std::shared_ptr<PolarNode> read_v1(const std::string &filename, const std::string &root_name) {
    netCDF::NcFile datafile(filename, netCDF::NcFile::read);

    std::string vessel_name;
    if (root_name == "from_group") {
      datafile.getAtt("vessel_name").getValues(vessel_name);
    } else {
      vessel_name = root_name;
    }

    auto root_node = read_polar_node(datafile, root_name);
    datafile.close();

    // vessel_name

//    auto operation_mode = std::make_shared<PolarNode>("");
//    NIY_POEM
//    return operation_mode;
    return root_node;
  }

  std::shared_ptr<PolarNode> read_poem_nc_file(const std::string &filename, const std::string &root_name) {

    spdlog::info("Reading file: {}", filename);

    int major_version = get_version(filename);

    spdlog::info("POEM file format version detected: v{}", major_version);
    if (!spec_check(filename, major_version)) {
      spdlog::critical("File is not compliant with version v{}", major_version);
      CRITICAL_ERROR_POEM
    } else {
      spdlog::info("File is compliant with version v{}", major_version);
    }

    std::shared_ptr<PolarNode> root_group;
    switch (major_version) {
      case 0:
        root_group = read_v0(filename, root_name);
        break;
      case 1:
        root_group = read_v1(filename, root_name);
        break;
    }

    return root_group;
  }

  POLAR_NODE_TYPE guess_polar_node_type(const netCDF::NcGroup &group) {
    POLAR_NODE_TYPE polar_node_type = POLAR_NODE;

    // Is this a PolarSet?
    if (group.getGroups().contains("MPPP")
        || group.getGroups().contains("HPPP")
        || group.getGroups().contains("MVPP")
        || group.getGroups().contains("HVPP")
        || group.getGroups().contains("VPP")) {
      polar_node_type = POLAR_SET;

    } else if (group.getName() == "MPPP"
               || group.getName() == "HPPP"
               || group.getName() == "MVPP"
               || group.getName() == "HVPP"
               || group.getName() == "VPP") {
      polar_node_type = POLAR;

    } else {
      polar_node_type = POLAR_NODE;
    }

    return polar_node_type;
  }


}  // poem
