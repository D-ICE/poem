//
// Created by frongere on 07/02/25.
//

#include "check_v1.h"

#include <filesystem>
#include <dunits/dunits.h>

#include "poem/exceptions.h"
#include "semver/semver.hpp"

namespace fs = std::filesystem;

namespace poem::v1 {

  void not_compliant_warning(int rule) {
    LogUnsualWarningError(
        "----->> Not compliant with V1/R{}. "
        "See: https://dice-poem.readthedocs.io/en/latest/documentation/rules_v1.html#rule-v1-r{}",
        rule, rule);
  }

  std::vector<std::string> known_poem_node_types_groups = {"POLAR_NODE", "POLAR_SET", "POLAR"};

  std::vector<std::string> known_polar_modes = {"MPPP", "HPPP", "MVPP", "HVPP", "VPP"};

  template<class T>
  inline bool is_poem_object(const T &object) {
    return object.getAtts().contains("POEM_NODE_TYPE");
  }

  template<class T>
  inline std::string get_attribute(const T &object, const std::string &att_name) {
    // Warning, check of existence of attribute must be done outside of this function
    std::string att;
    object.getAtt(att_name).getValues(att);
    return att;
  }

  bool has_at_least_one_polar(const netCDF::NcGroup &group) {
    bool has_polar = false;
    if (is_poem_object(group)) {
      auto node_type = get_attribute(group, "POEM_NODE_TYPE");
      if (node_type == "POLAR") {
        has_polar = true;
      }
    }
    // Keep searching
    if (!has_polar) {
      for (const auto &group_: group.getGroups()) {
        has_polar = has_at_least_one_polar(group_.second);
        if (has_polar) break;
      }
    }

    return has_polar;
  }

  inline std::string group_name(const netCDF::NcGroup &group) {
    std::string group_name;
    if (group.isRootGroup()) {
      group_name = "root";
    } else {
      group_name = group.getName(true);
    }
    return group_name;
  }


  bool check_R1(const netCDF::NcGroup &group) {
    bool compliant = true;

    if (group.getAtts().contains("POEM_NODE_TYPE")) {

      // Parent of this group MUST be a POEM group
      if (!group.isRootGroup()) {
        if (!is_poem_object(group.getParentGroup())) {
          LogWarningError("In group {}, POEM_NODE_TYPE attribute found but not in parent group."
                          "Continuous POEM group is mandatory",
                          group_name(group));
          compliant = false;
        }
      }

      // Check group POEM_NODE_TYPE value
      std::string node_type = get_attribute(group, "POEM_NODE_TYPE");
      if (std::find(known_poem_node_types_groups.begin(), known_poem_node_types_groups.end(), node_type) ==
          known_poem_node_types_groups.end()) {
        LogWarningError("In group {}, POEM_NODE_TYPE attribute found but with bad value. "
                        "Excepted on of POLAR_NODE, POLAR_SET or POLAR. Found {}",
                        group_name(group), node_type);
        compliant = false;
      }

      // Variables
      for (const auto &nc_var: group.getVars()) {
        if (is_poem_object(nc_var.second)) {
          // Check that PolarTable and Dimension are in a Polar group
          if (node_type !=
              "POLAR") { // TODO: ajouter cette règle (pas de PolarTable ou Dimension in group other thant Polar)
            LogWarningError("In group {}, found Variable with attribute POEM_NODE_TYPE set to {}, "
                            "but group is not a POLAR.");
            compliant = false;
          }

          // Check Dimension and PolarTable POEM_NODE_TYPE value
          std::string var_node_type = get_attribute(nc_var.second, "POEM_NODE_TYPE");
          if (group.getCoordVars().contains(nc_var.second.getName())) {
            if (var_node_type != "POLAR_DIMENSION") {
              LogWarningError("In group {}, Coordinate Variable {} seen as a Dimension has incorrect "
                              "POEM_NODE_TYPE attribute. Expected POLAR_DIMENSION, found {}.",
                              group_name(group), nc_var.second.getName(), var_node_type);
              compliant = false;
            }
          } else {
            if (var_node_type != "POLAR_TABLE") {
              LogWarningError("In group {}, Variable {} seen as a PolarTable has incorrect "
                              "POEM_NODE_TYPE attribute. Expected POLAR_TABLE, found {}",
                              group_name(group), nc_var.second.getName(), var_node_type);
              compliant = false;
            }
          }
        }
      }
    }

    for (const auto &group_: group.getGroups()) {
      compliant &= check_R1(group_.second);
    }

    return compliant;
  }

  bool check_R2(const netCDF::NcGroup &group) { // TODO: verifier

    bool compliant = true;

    if (!group.isRootGroup()) {
      LogCriticalError("check_R2 function is expecting root group");
      CRITICAL_ERROR_POEM
    }

    // Root group mandatory attribute POEM_SPECIFICATION_VERSION
    if (group.getAtts().contains("POEM_SPECIFICATION_VERSION")) {
      std::string version_str = get_attribute(group, "POEM_SPECIFICATION_VERSION");
      int version = (int) semver::version::parse(version_str, false).major();
      if (version != 1) {
        LogWarningError("In root group, POEM_SPECIFICATION_VERSION attribute found but version number is {}."
                        "Version expected to check against is 1",
                        version);
        compliant = false;
      }
    } else {
      LogWarningError("In root group, POEM_SPECIFICATION_VERSION attribute not found.");
      compliant = false;
    }

    // Root group mandatory attribute VESSEL_NAME
    if (!group.getAtts().contains("VESSEL_NAME")) {
      LogWarningError("In root group, VESSEL_NAME attribute not found");
      compliant = false;
    }

    // Root group mandatory attribute POEM_NODE_TYPE
    if (!group.getAtts().contains("POEM_NODE_TYPE")) {
      LogWarningError("In root group, POEM_NODE_TYPE attribute not found");
      compliant = false;
    }

    return compliant;
  }

  bool check_R3(const netCDF::NcGroup &group) {

    bool compliant = true;

    if (is_poem_object(group)) {
      std::string node_type = get_attribute(group, "POEM_NODE_TYPE");

      // A PolarSet ALWAYS owns at least a Polar -> TODO: changer la regle en un PolarSet ne doit contenir que des Polar
      if (node_type == "POLAR_SET") {
        bool has_polar = false;
        for (const auto &group_: group.getGroups()) {
          if (is_poem_object(group_.second)) {
            has_polar = true;

            // A subgroup of a PolarSet MUST be a Polar
            auto node_type_ = get_attribute(group_.second, "POEM_NODE_TYPE");
            if (node_type_ != "POLAR") {
              LogWarningError("In group {} seen as a PolarSet, POEM subgroups must be of type Polar. "
                              "Found subgroup {} of type {}",
                              group_name(group), group_.second.getName(true), node_type_);
              compliant = false;
            }
          }
        }
        if (!has_polar) {
          LogWarningError("In group {} seen as a PolarSet, no Polar subgroup found", group_name(group));
          compliant = false;
        }
      }

      // A Polar ALWAYS owns at least a PolarTable with associated Dimensions
      if (node_type == "POLAR") {
        bool has_polar_table = false;
        std::vector<netCDF::NcDim> dims;
        std::string table_ref_name;
        for (const auto &nc_var: group.getVars()) {
          if (!group.getCoordVars().contains(nc_var.second.getName()) &&
              is_poem_object(nc_var.second)) {
            // This is a Variable representing a PolarTable

            // First PolarTable found, registering its name and dimensions for later comparison
            if (!has_polar_table) {
              dims = nc_var.second.getDims();
              table_ref_name = nc_var.first;
              has_polar_table = true;

              // Check that Dimensions are well present into the group
              for (const auto &dim: dims) {
                if (!group.getCoordVars().contains(dim.getName())) {
                  LogWarningError("In group {}, PolarTable {}'s Dimension {} not found in the group",
                                  group_name(group), nc_var.first, dim.getName());
                  compliant = false;
                }
              }
            }

            // Check Dimension consistency against ref PolarTable
            auto dims_ = nc_var.second.getDims();
            if (dims_ != dims) {
              LogWarningError("In group {}, enclosed PolarTables {} and {} have inconsistent "
                              "Dimensions definitions",
                              group_name(group), table_ref_name, nc_var.second.getName());
              compliant = false;
            }
          }
        }
        if (!has_polar_table) {
          LogWarningError("In group {}, seen as a Polar, no PolarTable found", group_name(group));
          compliant = false;
        }
      }

      // Iterating on subgroups
      for (const auto &group_: group.getGroups()) {
        compliant &= check_R3(group_.second);
      }

    }

    if (group.isRootGroup()) {
      // Checking that we have at least one Polar in the file
      if (!has_at_least_one_polar(group)) {
        LogWarningError("No Polar found in the file");
        compliant = false;
      }
    }

    return compliant;
  }

  bool check_R4(const netCDF::NcGroup &group) {
    bool compliant = true;

    if (is_poem_object(group)) {
      auto node_type = get_attribute(group, "POEM_NODE_TYPE");
      if (node_type == "POLAR") {

        if (group.getAtts().contains("POEM_MODE")) {

          auto poem_mode = get_attribute(group, "POEM_MODE");
          if (std::find(known_polar_modes.begin(), known_polar_modes.end(), poem_mode) == known_polar_modes.end()) {
            LogWarningError("In group {}, seen as a Polar, POEM_MODE attribute is not valid. "
                            "Expected either MPPP, HPPP, MVPP, HVPP or VPP. Found {}",
                            group_name(group), poem_mode);
            compliant = false;
          }

          if (!group.isRootGroup()) {
            // group name must equal to POEM_MODE
            if (group.getName() != poem_mode) {
              LogWarningError("In group {}, seen as a Polar and not a root group, group name MUST be "
                              "the same as underlying POEM_MODE attribute. Expected {}, found {}",
                              group_name(group), poem_mode, group.getName());
              compliant = false;
            }
          }
        } else {
          LogWarningError("In group {}, seen as a Polar, attribute POEM_MODE not found",
                          group_name(group));
          compliant = false;
        }

      }

      // Iterating on subgroups
      for (const auto &group_: group.getGroups()) {
        compliant &= check_R4(group_.second);
      }
    }

    return compliant;
  }

  bool check_R5(const netCDF::NcGroup &group) {
    bool compliant = true;

    if (is_poem_object(group)) {
      auto node_type = get_attribute(group, "POEM_NODE_TYPE");
      if (node_type == "POLAR") {
        for (const auto nc_var: group.getVars()) {
          if (is_poem_object(nc_var.second)) {
            if (!nc_var.second.getAtts().contains("unit")) {
              LogWarningError("In group {}, Variable {} doe not have unit attribute",
                              group_name(group), nc_var.first);
              compliant = false;
            }
            if (!nc_var.second.getAtts().contains("description")) {
              LogWarningError("In group {}, Variable {} doe not have description attribute",
                              group_name(group), nc_var.first);
              compliant = false;
            }
          }
        }

        return compliant;
      }

      // Iterating on subgroups
      for (const auto &group_: group.getGroups()) {
        compliant &= check_R5(group_.second);
      }

    }

    return compliant;
  }

  bool check_R6(const netCDF::NcGroup &group) {
    bool compliant = true;

    if (is_poem_object(group)) {
      auto node_type = get_attribute(group, "POEM_NODE_TYPE");
      if (node_type == "POLAR") {
        for (const auto &coord_var: group.getCoordVars()) {
          auto var = group.getVar(coord_var.first);
          if (!is_poem_object(var)) continue;

          std::vector<double> values(var.getDim(0).getSize());
          var.getVar(values.data());
          double front_value = values.front();

          // Positive values
          if (front_value < 0.) {
            LogWarningError("In group {}, values for Dimension {} MUST be positive. Found {}",
                            group_name(group), var.getName(), front_value);
            compliant = false;
          }

          // Strictly increasing values
          front_value -= 1.;
          for (const auto &val: values) {
            if (val <= front_value) {
              LogWarningError("In group {}, values for Dimension {} MUST be strictly increasing. "
                              "Found {} <= {}.", group_name(group), var.getName(), val, front_value);
              compliant = false;
            }
            front_value = val;
          }

          // Not higher than 180 for angular values
          auto unit = get_attribute(var, "unit");
          if (unit == "deg") {
            if (values.back() > 180.) {
              LogWarningError("In group {}, values for angular Dimension {} MUST be between 0 and 180 deg. "
                              "Found {}",
                              group_name(group), var.getName(), values.back());
              compliant = false;
            }
          }

        }

      } else {
        for (const auto &group_: group.getGroups()) {
          compliant &= check_R6(group_.second);
        }
      }

    }

    return compliant;

  }

  bool has_coord_var(const netCDF::NcGroup &group, const std::string &name) {
    bool compliant = true;

    std::string node_type = get_attribute(group, "POEM_NODE_TYPE");
    if (node_type != "POLAR") {
      LogCriticalError("Group {} must be a POLAR, found a {}", group_name(group), node_type);
      CRITICAL_ERROR_POEM
    }

    if (group.getCoordVars().contains(name)) {
      auto var = group.getVar(name);
      if (var.getAtts().contains("POEM_NODE_TYPE")) {
        std::string var_type;
        var.getAtt("POEM_NODE_TYPE").getValues(var_type);
        if (var_type != "POLAR_DIMENSION") {
          LogWarningError("In {}, mandatory Coordinate Variable {} found but its attribute POEM_NODE_TYPE "
                          "is not POLAR_DIMENSION. Found {}",
                          group_name(group), name, var_type);
          compliant = false;
        }
      } else {
        LogWarningError("In {}, mandatory Coordinate Variable {} is found, but attribute POEM_NODE_TYPE is not found",
                        group_name(group), name);
        compliant = false;
      }
      // TODO: verifier qu'on a bien POEM_NODE_TYPE qui est DIMENSION_NODE
    } else {
      auto polar_mode = get_attribute(group, "POEM_MODE");
      LogWarningError("In group {} (with mode {}), mandatory Coordinate Variable {} not found",
                      group_name(group), polar_mode, name);
      compliant = false;
    }

    return compliant;
  }

  bool has_var(const netCDF::NcGroup &group, const std::string &name, const std::vector<std::string> &dims) {
    bool compliant = true;

    if (group.getVars().contains(name)) {
      if (group.getCoordVars().contains(name)) {
        LogWarningError("In group {}, Variable {} should not be a Coordinate Variable",
                        group_name(group), name);
        compliant = false;
        // FIXME: cette condition n'existe pas dans les specs
      }
      auto nc_var = group.getVar(name);
      if (nc_var.getAtts().contains("POEM_NODE_TYPE")) {
        std::string node_type = get_attribute(nc_var, "POEM_NODE_TYPE");
//        nc_var.getAtt("POEM_NODE_TYPE").getValues(node_type);
        if (node_type != "POLAR_TABLE") {
          LogWarningError("In group {}, mandatory variable {} found but its attribute "
                          "POEM_NODE_TYPE is not POLAR_TABLE. Found {}",
                          group_name(group), name, node_type);
          compliant = false;
        }
      } else {
        LogWarningError("In group {}, mandatory variable {} found but has no attribute POEM_NODE_TYPE",
                        group_name(group), name);
        compliant = false;
      }

      auto var_dims = nc_var.getDims();
      if (var_dims.size() != dims.size()) {
        LogWarningError("In group {}, mandatory variable {} has not the correct number of dimensions. "
                        "Expected {}, found {}",
                        group_name(group), name, dims.size(), var_dims.size());
        compliant = false;
      }
      for (size_t i = 0; i < dims.size(); ++i) {
        if (dims[i] != var_dims[i].getName()) {
          LogWarningError("In group {}, mandatory variable {} has incorrect dimension {}. "
                          "Expected {}, found {}",
                          group_name(group), name, i, dims[i], var_dims[i].getName());
          compliant = false;
        }
      }

      // TODO: verifier qu'on a bien toutes les dims dans le bon ordre et que la variable est a bien un POEM_NODE_TYPE
      //  a POLAR_TABLE
    } else {
      std::string polar_mode = get_attribute(group, "POEM_MODE");
//      group.getAtt("POLAR_MODE").getValues(polar_mode);
      LogWarningError("In group {} (with mode {}), mandatory Variable {} not found",
                      group_name(group), polar_mode, name);
      compliant = false;
    }

    return compliant;
  }

  bool check_R7(const netCDF::NcGroup &group) {
    bool compliant = true;

    if (is_poem_object(group)) {
      std::string node_type = get_attribute(group, "POEM_NODE_TYPE");

      if (node_type == "POLAR") {
        auto polar_mode = get_attribute(group, "POEM_MODE");

        if (polar_mode == "MPPP" || polar_mode == "HPPP") {
          compliant &= has_coord_var(group, "STW_dim");
          compliant &= has_coord_var(group, "TWS_dim");
          compliant &= has_coord_var(group, "TWA_dim");
          compliant &= has_coord_var(group, "WA_dim");
          compliant &= has_coord_var(group, "Hs_dim");
          compliant &= has_var(group, "TOTAL_POWER", {"STW_dim", "TWS_dim", "TWA_dim", "WA_dim", "Hs_dim"});
          compliant &= has_var(group, "LEEWAY", {"STW_dim", "TWS_dim", "TWA_dim", "WA_dim", "Hs_dim"});
          compliant &= has_var(group, "SOLVER_STATUS", {"STW_dim", "TWS_dim", "TWA_dim", "WA_dim", "Hs_dim"});
        }
        if (polar_mode == "MVPP" || polar_mode == "HVPP") {
          compliant &= has_coord_var(group, "Power_dim");
          compliant &= has_coord_var(group, "TWS_dim");
          compliant &= has_coord_var(group, "TWA_dim");
          compliant &= has_coord_var(group, "WA_dim");
          compliant &= has_coord_var(group, "Hs_dim");
          compliant &= has_var(group, "STW", {"Power_dim", "TWS_dim", "TWA_dim", "WA_dim", "Hs_dim"});
          compliant &= has_var(group, "LEEWAY", {"Power_dim", "TWS_dim", "TWA_dim", "WA_dim", "Hs_dim"});
          compliant &= has_var(group, "SOLVER_STATUS",
                               {"Power_dim", "TWS_dim", "TWA_dim", "WA_dim", "Hs_dim"});
        }
        if (polar_mode == "VPP") {
          compliant &= has_coord_var(group, "TWS_dim");
          compliant &= has_coord_var(group, "TWA_dim");
          compliant &= has_coord_var(group, "WA_dim");
          compliant &= has_coord_var(group, "Hs_dim");
          compliant &= has_var(group, "STW", {"TWS_dim", "TWA_dim", "WA_dim", "Hs_dim"});
          compliant &= has_var(group, "LEEWAY", {"TWS_dim", "TWA_dim", "WA_dim", "Hs_dim"});
          compliant &= has_var(group, "SOLVER_STATUS", {"TWS_dim", "TWA_dim", "WA_dim", "Hs_dim"});
        }

      } else {
        for (const auto &group_: group.getGroups()) {
          compliant &= check_R7(group_.second);
        }
      }
    }

    return compliant;
  }

}

bool poem::check_v1(const std::string &filename) {

  if (!fs::exists(filename)) {
    LogCriticalError("NetCDF file not found: {}", filename);
    CRITICAL_ERROR_POEM
  }

  netCDF::NcFile root_group(filename, netCDF::NcFile::read);

  bool compliant = true;

  if (!v1::check_R1(root_group)) {
    v1::not_compliant_warning(1);
    compliant = false;
  }

  if (!v1::check_R2(root_group)) {
    v1::not_compliant_warning(2);
    compliant = false;
  }

  if (!v1::check_R3(root_group)) {
    v1::not_compliant_warning(3);
    compliant = false;
  }

  if (!v1::check_R4(root_group)) {
    v1::not_compliant_warning(4);
    compliant = false;
  }

  if (!v1::check_R5(root_group)) {
    v1::not_compliant_warning(5);
    compliant = false;
  }

  if (!v1::check_R6(root_group)) {
    v1::not_compliant_warning(6);
    compliant = false;
  }

  if (!v1::check_R7(root_group)) {
    v1::not_compliant_warning(7);
    compliant = false;
  }

  root_group.close();

  return compliant;

}  // poem::v1
