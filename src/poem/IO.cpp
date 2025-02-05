//
// Created by frongere on 21/01/25.
//

#include "IO.h"

#include <semver/semver.hpp>
#include <datetime.h>

#include "PolarTable.h"
#include "Polar.h"
#include "PolarSet.h"
#include "specifications/spec_checkers.h"

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
      group.putAtt(attribute.first, attribute.second);
    }
  }

  void to_netcdf(const Attributes &attributes, netCDF::NcVar &nc_var) {
    for (const auto &attribute: attributes) {
      nc_var.putAtt(attribute.first, attribute.second);
    }
  }

  void to_netcdf(std::shared_ptr<Polar> polar, netCDF::NcGroup &group) {
    for (const auto &polar_table: polar->children<PolarTableBase>()) {
      to_netcdf(polar_table, group);
    }

    if (!polar->attributes().contains("polar_mode")) {
      group.putAtt("polar_mode", polar_mode_to_string(polar->mode()));
    }
    to_netcdf(polar->attributes(), group);
  }

  void to_netcdf(std::shared_ptr<PolarSet> polar_set, netCDF::NcGroup &group) {
    for (const auto &polar: polar_set->children<Polar>()) {
      auto new_group = group.addGroup(polar_mode_to_string(polar->mode()));
      to_netcdf(polar, new_group);
    }
    to_netcdf(polar_set->attributes(), group);
  }

  void to_netcdf(std::shared_ptr<PolarNode> polar_node, netCDF::NcGroup &group) {

    switch (polar_node->polar_node_type()) {

      case POLAR_NODE: {
        for (const auto &next_polar_node: polar_node->children<PolarNode>()) {
          auto new_group = group.addGroup(next_polar_node->name());
          to_netcdf(next_polar_node, new_group);
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

  void to_netcdf(std::shared_ptr<PolarNode> polar_node, const std::string &vessel_name, const std::string &filename) {
    LogNormalInfo("Writing file <specification v{}>: {}",
                  current_poem_standard_version(),
                  fs::absolute(filename).string());

    netCDF::NcFile root_group(filename, netCDF::NcFile::replace);
    to_netcdf(polar_node, root_group);
    root_group.putAtt("POEM_LIBRARY_VERSION", git::version_full());
    root_group.putAtt("POEM_SPECIFICATION_VERSION", "v" + std::to_string(current_poem_standard_version()));
    root_group.putAtt("date", jed_utils::datetime().to_string("yyyy-MM-dd HH:mm:ss tt"));
    root_group.putAtt("vessel_name", vessel_name);

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

  // TODO: ne conserver que des read, renvoyer void et passer les shared_ptr... (sauf pour read_dimension_grid)

  void read_dimension_grid_from_var(const netCDF::NcVar &var, std::shared_ptr<DimensionGrid> dimension_grid) {

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
    dimension_grid = make_dimension_grid(dimension_set);

    for (size_t idim = 0; idim < ndim; ++idim) {
      auto name = dimension_set->dimension(idim)->name();
      auto dim_var = group.getVar(name);
      std::vector<double> values(dim_var.getDim(0).getSize());
      dim_var.getVar(values.data());

      dimension_grid->set_values(name, values);

    }

  }

  void read_polar_table(const netCDF::NcVar &var,
                        std::shared_ptr<PolarTableBase> polar_table,
                        std::shared_ptr<DimensionGrid> &dimension_grid,
                        bool dimension_grid_from_var) {

    if (dimension_grid_from_var) {
      read_dimension_grid_from_var(var, dimension_grid);
    }

    if (var.getDimCount() != dimension_grid->ndims()) {
      LogCriticalError("Dimension mismatch between netCDF dataset var and DimensionGrid");
      CRITICAL_ERROR_POEM
    }

    size_t ndim = dimension_grid->ndims();

    std::vector<std::shared_ptr<Dimension>> dimensions(ndim);
    for (size_t idim = 0; idim < ndim; ++idim) {
      if (var.getDim((int) idim).getName() != dimension_grid->dimension_set()->dimension(idim)->name()) {
        LogCriticalError("Dimension name mismatch between netCDF dataset and DimensionGrid");
        CRITICAL_ERROR_POEM
      }
    }

    std::string name = var.getName();
    std::string unit;
    var.getAtt("unit").getValues(unit);
    std::string description;
    var.getAtt("description").getValues(description);

//    std::shared_ptr<PolarTableBase> polar_table;

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
        LogCriticalError("Type not supported");
        CRITICAL_ERROR_POEM
    }

    // TODO: lire les attributs

//    return polar_table;

  }

  void read_polar(const netCDF::NcGroup &group,
                  std::shared_ptr<Polar> polar,
                  const std::string &name) {

    std::string name_;
    if (name == "from-group") {
      if (group.isRootGroup()) {
        LogCriticalError("While reading PolarNode, when group is root, the polar node name must be specified as"
                         "it cannot be retrieved from group name");
        CRITICAL_ERROR_POEM
      }
      name_ = group.getName();
    } else {
      name_ = name;
    }

    // C'est dans le read_v0 qu'on doit gerer le nom en HPPP ou MPPP

//
//    NIY_POEM
////    auto polar_name = group.getName();
//
//    POLAR_MODE polar_mode;
//    if (!is_polar_mode(polar_name)) {
//      // Trying to infer polar_mode from group name
//
//      if (group.getAtts().contains("polar_mode")) {
//        group.getAtt("polar_mode").getValues(polar_name);
//        polar_mode = string_to_polar_mode(polar_name);
//
//      } else if (group.getAtts().contains("vessel_type")) {
//        // For v0...
//        std::string vessel_type;
//        group.getAtt("vessel_type").getValues(vessel_type);
//        if (vessel_type == "HYBRID") {
//          polar_mode = HPPP;
//        } else if (vessel_type == "MOTOR") {
//          polar_mode = MPPP;
//        } else {
//          LogCriticalError("While reading file with format v0, vessel_type {} unknown");
//          CRITICAL_ERROR_POEM
//        }
//
//      } else {
//        LogCriticalError("Cannot infer the POLAR_MODE of group {} while reading Polar", polar_name);
//        CRITICAL_ERROR_POEM
//      }
//    } else {
//      polar_mode = string_to_polar_mode(polar_name);
//    }

    POLAR_MODE polar_mode = string_to_polar_mode(name_);

    std::shared_ptr<DimensionGrid> dimension_grid;
    polar = make_polar(name, polar_mode, dimension_grid);

    bool dimension_grid_from_var = true;
    for (const auto &nc_var: group.getVars()) {
      if (group.getCoordVars().contains(nc_var.first)) continue;

      std::shared_ptr<PolarTableBase> polar_table;
      read_polar_table(nc_var.second, polar_table, dimension_grid, dimension_grid_from_var);

      if (dimension_grid_from_var) {
        polar->dimension_grid() = dimension_grid;
        dimension_grid_from_var = false;
      }

      polar->attach_polar_table(polar_table);
    }

    // TODO: lire les attributs

  }

  void read_polar_set(const netCDF::NcGroup &group,
                      std::shared_ptr<PolarSet> polar_set,
                      const std::string &polar_set_name) {

//    NIY_POEM

    std::string polar_set_name_;
    if (polar_set_name == "from-group") {
      if (group.isRootGroup()) {
        LogCriticalError("While reading PolarSet, if the given group is root, the name must be specified");
        CRITICAL_ERROR_POEM
      } else {
        polar_set_name_ = group.getName();
      }
    } else {
      polar_set_name_ = polar_set_name;
    }

    polar_set = std::make_shared<PolarSet>(polar_set_name_);

    for (const auto &subgroup: group.getGroups()) {
      if (!is_polar_mode(subgroup.first)) continue; // Group name for Polar MUST be a POLAR_MODE string representation

      std::shared_ptr<Polar> polar;
      read_polar(subgroup.second, polar);

      polar_set->attach_polar(polar);

    }

    // TODO: lire les attributs

  }

  void read_polar_node(const netCDF::NcGroup &group,
                       std::shared_ptr<PolarNode> polar_node,
                       const std::string &polar_node_name) {

    // FIXME: c'est a priori plus sioux que ca is root ou pas... surtout quand on itere en recursif

    /*
     * group is_root & from-group -> non
     * group is_root & name -> utilise name
     * group !is_root & from-group
     */

//    std::string polar_node_name;
//    if (polar_node_name_ == "from-file") {
//      if (group.isRootGroup()) {
//        LogCriticalError("While reading PolarNode, when group is root, the polar node name must be specified as"
//                         "it cannot be retrieved from group name");
//        CRITICAL_ERROR_POEM
//      }
//      polar_node_name = group.getName();
//    } else {
//      polar_node_name = polar_node_name_;
//    }

//    std::shared_ptr<PolarNode> polar_node;

//    if (group.isRootGroup()) {
//
//    }
    // FIXME: il faut lire les attributs... notamment vessel_name

    auto polar_mode_type = guess_polar_node_type(group);
    switch (polar_mode_type) {
      case POLAR_NODE: {
        polar_node = make_polar_node(polar_node_name);
        for (const auto &subgroup: group.getGroups()) {
          std::shared_ptr<PolarNode> new_polar_node;
          read_polar_node(subgroup.second, new_polar_node);
          new_polar_node->attach_to(polar_node);
        }
        break;
      }
      case POLAR_SET: {
        std::shared_ptr<PolarSet> polar_set;
        read_polar_set(group, polar_set, polar_node_name);
        polar_node = polar_set;
        break;
      }
      case POLAR: {
        std::shared_ptr<Polar> polar;
        read_polar(group, polar, polar_node_name);
        polar_node = polar;
        break;
      }
      default:
        LogCriticalError("Something went wrong, we should never be here...");
        CRITICAL_ERROR_POEM
    }

    // FIXME: voir si on fait ca...
//    if (group.isRootGroup()) {
//      polar_node->attributes().add_attribute("vessel_name", polar_node_name);
//    }

    // TODO: lire les attributs

  }


  void read_v0(const netCDF::NcGroup &root_group,
               std::shared_ptr<PolarNode> root_node,
               const std::string &root_name) {

    // PolarNode is root and we set the vessel name as filename as we do not have vessel name in v0
//    std::string vessel_name = fs::path(filename).stem();
    if (root_name == "from-file") {
      LogCriticalError("While reading POEM file version v0, "
                       "root_name must be specified and must be vessel_name");
      CRITICAL_ERROR_POEM
    }

    // Get the polar_name
//    netCDF::NcFile datafile(filename, netCDF::NcFile::read);

    POLAR_MODE polar_mode;
    std::string vessel_type;
    root_group.getAtt("vessel_type").getValues(vessel_type);
    if (vessel_type == "HYBRID") {
      polar_mode = HPPP;
    } else if (vessel_type == "MOTOR") {
      polar_mode = MPPP;
    } else {
      LogCriticalError("While reading file with format v0, vessel_type {} unknown");
      CRITICAL_ERROR_POEM
    }

    // On voudrait avoir /vessel/MPPP/polar_tables...
    // /PolarSet/Polar/PolarTables...

    root_node = make_polar_set(root_name);
    std::shared_ptr<Polar> polar;
    read_polar(root_group, polar, polar_mode_to_string(polar_mode));
//    datafile.close();

    polar->attach_to(root_node); // FIXME: on doit forcement avoir un PolarSet dans la lecture v0 ???

    root_node->attributes().add_attribute("vessel_name", root_name);

  }

  void read_v1(const netCDF::NcGroup &root_group,
               std::shared_ptr<PolarNode> root_node,
               const std::string &root_name) {
//    root_group, root_name, root_node

    std::string vessel_name;
    if (root_name == "from-file") {
      root_group.getAtt("vessel_name").getValues(vessel_name);
    } else {
      vessel_name = root_name;
    }

    read_polar_node(root_group, root_node, vessel_name);


//    for ()

//    datafile.close();

    NIY_POEM
    // vessel_name

//    auto operation_mode = std::make_shared<PolarNode>("");
//    NIY_POEM
//    return operation_mode;
  }

  std::shared_ptr<PolarNode> read_poem_nc_file(const std::string &filename, const std::string &root_name) {

    LogNormalInfo("Reading file: {}", fs::absolute(filename).string());
    int major_version = get_version(filename);
    LogNormalInfo("POEM specification v{} detected in file", major_version);

    // Check compliancy with specification
    if (!spec_check(filename, major_version)) {
      LogCriticalError("File is not compliant with version v{}", major_version);
      CRITICAL_ERROR_POEM
    } else {
      LogNormalInfo("File is compliant with version v{}", major_version);
    }

    netCDF::NcFile root_group(filename, netCDF::NcFile::read);
    std::shared_ptr<PolarNode> root_node;
    switch (major_version) {
      case 0:
        read_v0(root_group, root_node, root_name);
        break;
      case 1:
        read_v1(root_group, root_node, root_name);
        break;
      default:
        LogCriticalError("Specification version v{} not known", major_version);
        CRITICAL_ERROR_POEM
    }
    root_group.close();

    return root_node;
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
