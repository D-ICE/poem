//
// Created by frongere on 21/01/25.
//

#include "IO.h"

#include <semver/semver.hpp>
#include <datetime.h>

#include "PolarTable.h"
#include "Polar.h"
#include "PolarSet.h"
#include "specifications/specs.h"

namespace poem {

  // ===================================================================================================================
  // WRITERS
  // ===================================================================================================================

  int current_poem_standard_version() {
    return git::version_major();
  }

  std::vector<netCDF::NcDim> to_netcdf(std::shared_ptr<DimensionGrid> dimension_grid,
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
        nc_var.putAtt("POEM_NODE_TYPE", "POLAR_DIMENSION");
      }

    } else {
      // Checking that dimensions and CoordVars of the group are consistent with specified DimensionGrid
      for (const auto &dimension: *dimension_grid->dimension_set()) {
        // Does the group has this dimension?
        if (!group.getDims().contains(dimension->name())) {
          LogCriticalError("In group {}, dimension {} has no corresponding CoordVar",
                           group.getName(), dimension->name());
          CRITICAL_ERROR_POEM
        }
        // Does the group has the corresponding variable?
        if (!group.getVars().contains(dimension->name())) {
          LogCriticalError("Group {} does not have a CoordVar associated to dimension {}",
                           group.getName(), dimension->name());
        }
        // Does the dimension has the correct size?
        if (group.getDim(dimension->name()).getSize() != dimension_grid->size(dimension->name())) {
          LogCriticalError("Group {} dimension {} has inconsistent dimension size. Found {} expected {}",
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
          LogCriticalError("Coord Variable {} of group {} has inconsistent values with specified DimensionGrid",
                           nc_var.getName(), group.getName());
          CRITICAL_ERROR_POEM
        }

        // Same unit and description?
        std::string unit;
        nc_var.getAtt("unit").getValues(unit);
        if (unit != dimension->unit()) {
          LogCriticalError("Group {} dimension {} has not the same unit as the one of specified DimensionGrid",
                           group.getName(), dimension->name());
          CRITICAL_ERROR_POEM
        }

        std::string description;
        nc_var.getAtt("description").getValues(description);
        if (description != dimension->description()) {
          LogCriticalError("Group {} dimension {} has not the same description as the one of specified DimensionGrid",
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
      if (group.getAtts().contains(attribute.first)) {
        // Check that we do have the same value for this attribute!
        std::string att_value;
        group.getAtt(attribute.first).getValues(att_value);
        if (att_value != attribute.second) {
          LogWarningError("In group {}, identical attribute {} with different values {} and {}",
                          group.getName(true), attribute.first, att_value, attribute.second);
        }
      } else {
        group.putAtt(attribute.first, attribute.second);
      }
    }
  }

  void to_netcdf(const Attributes &attributes, netCDF::NcVar &nc_var) {
    for (const auto &attribute: attributes) {
      if (nc_var.getAtts().contains(attribute.first)) {
        // Check that we do have the same value for this attribute!
        std::string att_value;
        nc_var.getAtt(attribute.first).getValues(att_value);
        if (att_value != attribute.second) {
          LogWarningError("In group {}, Variable {}, identical attribute {} with different values {} and {}",
                          nc_var.getParentGroup().getName(true),
                          nc_var.getName(),
                          attribute.first,
                          att_value, attribute.second);
        }
      } else {
        nc_var.putAtt(attribute.first, attribute.second);
      }
    }
  }

  void to_netcdf(std::shared_ptr<Polar> polar, netCDF::NcGroup &group) {
    for (const auto &polar_table: polar->children<PolarTableBase>()) {
      to_netcdf(polar_table, group);
    }

    to_netcdf(polar->attributes(), group);
    group.putAtt("POEM_NODE_TYPE", "POLAR");
    group.putAtt("POEM_MODE", polar_mode_to_string(polar->mode()));
    group.putAtt("description", polar->description());
  }

  void to_netcdf(std::shared_ptr<PolarSet> polar_set, netCDF::NcGroup &group) {
    for (const auto &polar: polar_set->children<Polar>()) {
      auto new_group = group.addGroup(polar_mode_to_string(polar->mode()));
      to_netcdf(polar, new_group);
    }
    to_netcdf(polar_set->attributes(), group);
    group.putAtt("POEM_NODE_TYPE", "POLAR_SET");
    group.putAtt("description", polar_set->description());
  }

  void to_netcdf(std::shared_ptr<PolarNode> polar_node, netCDF::NcGroup &group) {

    switch (polar_node->polar_node_type()) {

      case POLAR_NODE: {
        for (const auto &next_polar_node: polar_node->children<PolarNode>()) {
          auto new_group = group.addGroup(next_polar_node->name());
          to_netcdf(next_polar_node, new_group);
        }
        group.putAtt("POEM_NODE_TYPE", "POLAR_NODE");
        group.putAtt("description", polar_node->description());
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
        auto polar_table = polar_node->as_polar_table();
        auto type = polar_table->type();

        switch (type) {
          case POEM_DOUBLE:
            to_netcdf(polar_table->as_polar_table_double(), netCDF::ncDouble, group);
            break;

          case POEM_INT:
            to_netcdf(polar_table->as_polar_table_int(), netCDF::ncInt, group);
            break;

          default:
            LogCriticalError("Type not supported");
            CRITICAL_ERROR_POEM
        }
        break;
      }

    }

  }

  void to_netcdf(std::shared_ptr<PolarNode> polar_node,
                 const std::string &vessel_name,
                 const std::string &filename,
                 bool verbose) {
    if (verbose)
      LogNormalInfo("Writing file <v{}>: {}",
                    current_poem_standard_version(),
                    fs::absolute(filename).string());

    netCDF::NcFile root_group(filename, netCDF::NcFile::replace);
    to_netcdf(polar_node, root_group);
    root_group.putAtt("POEM_LIBRARY_VERSION", git::version_full());
    root_group.putAtt("POEM_SPECIFICATION_VERSION", "v" + std::to_string(current_poem_standard_version()));
    root_group.putAtt("date", jed_utils::datetime().to_string("yyyy-MM-dd HH:mm:ss tt"));
    root_group.putAtt("VESSEL_NAME", vessel_name);

    root_group.close();
  }

  // ===================================================================================================================
  // READERS
  // ===================================================================================================================

  int get_version(const std::string &filename) {
    if (!fs::exists(filename)) {
      LogCriticalError("NetCDF file not found: {}", filename);
      CRITICAL_ERROR_POEM
    }

    netCDF::NcFile datafile(std::string(filename), netCDF::NcFile::read);

    int major_version;
    if (datafile.getAtts().contains("POEM_SPECIFICATION_VERSION")) {
      std::string spec_version;
      datafile.getAtt("POEM_SPECIFICATION_VERSION").getValues(spec_version);
      major_version = (int) semver::version::parse(spec_version, false).major();

    } else {
      // This is likely to be a file with version v0
      major_version = 0;
    }
    datafile.close();

    return major_version;
  }

  std::vector<std::string> excluded_attributes = {
      "unit",
      "description",
      "POEM_NODE_TYPE",
      "POEM_MODE",
      "POEM_LIBRARY_VERSION",
      "POEM_SPECIFICATION_VERSION",
      "VESSEL_NAME",
      "date"
  };

  template<class T>
  void read_attributes(const T &nc_object, std::shared_ptr<PolarNode> polar_node) {
    for (const auto &att: nc_object.getAtts()) {
      if (std::find(excluded_attributes.begin(), excluded_attributes.end(), att.first) !=
          excluded_attributes.end())
        continue;
      std::string val;
      nc_object.getAtt(att.first).getValues(val);
      polar_node->attributes().add_attribute(att.first, val);
    }
  }

  std::shared_ptr<PolarNode> load_v0(const netCDF::NcGroup &root_group,
                                     const std::string &root_name) {

    std::unordered_map<std::string, std::string> dimension_map{
        {"STW_kt",  "STW_dim"},
        {"TWS_kt",  "TWS_dim"},
        {"TWA_deg", "TWA_dim"},
        {"WA_deg",  "WA_dim"},
        {"Hs_m",    "Hs_dim"},
    };

    std::unordered_map<std::string, std::string> polar_tables_map{
        {"BrakePower",   "TOTAL_POWER"},
        {"SolverStatus", "SOLVER_STATUS"}
    };

    if (!root_group.isRootGroup()) {
      LogCriticalError("In load_v0, not a root group");
      CRITICAL_ERROR_POEM
    }

    std::string vessel_name;
    if (vessel_name == "from-file") {
      root_group.getAtt("VESSEL_NAME").getValues(vessel_name);
    } else {
      vessel_name = root_name;
    }

    // Getting polar mode
    std::string vessel_type;
    root_group.getAtt("vessel_type").getValues(vessel_type);
    POLAR_MODE polar_mode;
    if (vessel_type == "HYBRID") {
      polar_mode = HPPP;
    } else if (vessel_type == "MOTOR") {
      polar_mode = MPPP;
    }

    // Load variables
    std::shared_ptr<DimensionGrid> dimension_grid;
    std::vector<std::shared_ptr<PolarTableBase>> polar_tables;
    for (const auto &nc_var: root_group.getVars()) {
//      if (!nc_var.second.getAtts().contains("POEM_NODE_TYPE")) continue;

      if (!root_group.getCoordVars().contains(nc_var.first)) {
        // This is not a Coordinate Variable

        if (!dimension_grid) {
          // Dimension grid not built, building it!
          auto nc_dims = nc_var.second.getDims();
          std::vector<std::shared_ptr<Dimension>> dimensions;
          dimensions.reserve(nc_dims.size());

          for (const auto &nc_dim: nc_dims) {
            auto var_dim = root_group.getVar(nc_dim.getName());
            std::string unit;
            var_dim.getAtt("unit").getValues(unit);
            std::string description;
            var_dim.getAtt("description").getValues(description);
            auto dim_ = make_dimension(nc_dim.getName(), unit, description);
            if (dimension_map.contains(dim_->name())) {
              dim_->change_name(dimension_map[dim_->name()]);
            }
            dimensions.push_back(dim_);
          }
          auto dimension_set = make_dimension_set(dimensions);
          dimension_grid = make_dimension_grid(dimension_set);

          for (const auto &nc_dim: nc_dims) {
            auto var_dim = root_group.getVar(nc_dim.getName());
            std::vector<double> values(nc_dim.getSize());
            var_dim.getVar(values.data());
            dimension_grid->set_values(dimension_map[nc_dim.getName()], values);
          }

        }  // end building DimensionGrid

        std::string unit;
        nc_var.second.getAtt("unit").getValues(unit);
        std::string description;
        nc_var.second.getAtt("description").getValues(description);

        std::shared_ptr<PolarTableBase> polar_table;
        switch (nc_var.second.getType().getTypeClass()) {
          case netCDF::NcType::nc_DOUBLE:
            polar_table = make_polar_table_double(nc_var.first, unit, description, dimension_grid);
            break;
          case netCDF::NcType::nc_INT:
            polar_table = make_polar_table_int(nc_var.first, unit, description, dimension_grid);
            break;
          default:
            LogWarningError("In group {}, PolarTable {} of type {} not managed by POEM. Skip...",
                            root_group.getName(true), nc_var.first, nc_var.second.getType().getTypeClassName());
            continue;
        }
        read_attributes(nc_var.second, polar_table);

        if (polar_tables_map.contains(polar_table->name())) {
          polar_table->change_name(polar_tables_map[polar_table->name()]);
        }
        polar_tables.push_back(polar_table);

      }

    }

    auto polar = make_polar(vessel_name, polar_mode, dimension_grid);

    for (const auto &polar_table: polar_tables) {
      polar->add_child(polar_table);
    }

    return polar;
  }

  std::shared_ptr<PolarNode> load_group(const netCDF::NcGroup &group,
                                        std::string group_name = "from-group") {

    std::shared_ptr<PolarNode> polar_node;

    if (group.getAtts().contains("POEM_NODE_TYPE")) {

      if (group_name == "from-group") {
        if (group.isRootGroup()) {
          LogCriticalError("In load_group, group_name must be explicitly specified for root group");
          CRITICAL_ERROR_POEM
        }
        group_name = group.getName();
      }

      std::string node_type;
      group.getAtt("POEM_NODE_TYPE").getValues(node_type);

      if (node_type == "POLAR") {

        std::shared_ptr<DimensionGrid> dimension_grid;
        std::vector<std::shared_ptr<PolarTableBase>> polar_tables;
        for (const auto &nc_var: group.getVars()) {
          if (!nc_var.second.getAtts().contains("POEM_NODE_TYPE")) continue;

          if (!group.getCoordVars().contains(nc_var.first)) {
            // This is not a Coordinate Variable

            if (!dimension_grid) {
              // Dimension grid not built, building it!
              auto nc_dims = nc_var.second.getDims();
              std::vector<std::shared_ptr<Dimension>> dimensions;
              dimensions.reserve(nc_dims.size());

              for (const auto &nc_dim: nc_dims) {
                auto var_dim = group.getVar(nc_dim.getName());
                std::string unit;
                var_dim.getAtt("unit").getValues(unit);
                std::string description;
                var_dim.getAtt("description").getValues(description);
                auto dim_ = make_dimension(nc_dim.getName(), unit, description);
                dimensions.push_back(dim_);
              }
              auto dimension_set = make_dimension_set(dimensions);
              dimension_grid = make_dimension_grid(dimension_set);

              for (const auto &nc_dim: nc_dims) {
                auto var_dim = group.getVar(nc_dim.getName());
                std::vector<double> values(nc_dim.getSize());
                var_dim.getVar(values.data());
                dimension_grid->set_values(nc_dim.getName(), values);
              }

            }  // end building DimensionGrid

            std::string unit;
            nc_var.second.getAtt("unit").getValues(unit);
            std::string description;
            nc_var.second.getAtt("description").getValues(description);

            std::shared_ptr<PolarTableBase> polar_table;
            switch (nc_var.second.getType().getTypeClass()) {
              case netCDF::NcType::nc_DOUBLE:
                polar_table = make_polar_table_double(nc_var.first, unit, description, dimension_grid);
                break;
              case netCDF::NcType::nc_INT:
                polar_table = make_polar_table_int(nc_var.first, unit, description, dimension_grid);
                break;
              default:
                LogWarningError("In group {}, PolarTable {} of type {} not managed by POEM. Skip...",
                                group.getName(true), nc_var.first, nc_var.second.getType().getTypeClassName());
                continue;
            }
            read_attributes(nc_var.second, polar_table);
            polar_tables.push_back(polar_table);

          }
        }

        std::string polar_mode_str;
        group.getAtt("POEM_MODE").getValues(polar_mode_str);
        POLAR_MODE polar_mode = string_to_polar_mode(polar_mode_str);
        polar_node = make_polar(group_name, polar_mode, dimension_grid);

        // Attaching every PolarTable to the Polar
        for (const auto &polar_table: polar_tables) {
          polar_node->add_child(polar_table, false);
        }

        // END group is POLAR
      } else if (node_type == "POLAR_SET") {
        std::string description;
        group.getAtt("description").getValues(description);
        polar_node = make_polar_set(group_name, description);

      } else if (node_type == "POLAR_NODE") {
        std::string description;
        group.getAtt("description").getValues(description);
        polar_node = make_polar_node(group_name, description);

      }

      for (const auto &group_: group.getGroups()) {
        auto polar_node_ = load_group(group_.second);
        polar_node->add_child(polar_node_);
      }

    }

    read_attributes(group, polar_node);

    return polar_node;

  }

  std::shared_ptr<PolarNode> load_v1(const netCDF::NcGroup &root_group,
                                     const std::string &root_name) {

    if (!root_group.isRootGroup()) {
      LogCriticalError("In load_v1, not a root group");
      CRITICAL_ERROR_POEM
    }

    std::string vessel_name;
    if (root_name == "from-file") {
      root_group.getAtt("VESSEL_NAME").getValues(vessel_name);
    } else {
      vessel_name = root_name;
    }

    return load_group(root_group, vessel_name);
  }

  std::shared_ptr<PolarNode> load(const std::string &filename,
                                  const std::string &root_name,
                                  bool spec_checking,
                                  bool verbose) {

    if (verbose)
      LogNormalInfo("Reading file: {}", fs::absolute(filename).string());
    int major_version = get_version(filename);
    if (verbose)
      LogNormalInfo("POEM specification v{} detected in file", major_version);

    // Check compliancy with specification
    if (spec_checking) {
      if (!spec_check(filename, major_version)) {
        LogCriticalError("File is not compliant POEM Specification version {}", major_version);
        CRITICAL_ERROR_POEM
      } else {
        if (verbose)
          LogNormalInfo("File is compliant with version v{}", major_version);
      }
    }


    netCDF::NcFile root_group(filename, netCDF::NcFile::read);
    std::shared_ptr<PolarNode> root_node;
    switch (major_version) {
      case 0:
        root_node = load_v0(root_group, root_name);
        break;
      case 1: {
        try {
          root_node = load_v1(root_group, root_name);
          break;
        } catch (const PoemException &e) {
          LogCriticalError("Error while reading POEM File using specification v{}: {}",
                           major_version, fs::absolute(filename).string());
          LogCriticalError("Please spec check the file to get insight on the problem");
          throw e;
        }
      }

      default:
        LogCriticalError("Specification version v{} not known", major_version);
        CRITICAL_ERROR_POEM
    }
    root_group.close();

    return root_node;
  }

}  // poem
