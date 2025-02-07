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

  std::vector<std::string> known_poem_node_types = {"POLAR_NODE", "POLAR_SET", "POLAR", "POLAR_TABLE"};
  std::vector<std::string> known_polar_modes = {"MPPP", "HPPP", "MVPP", "HVPP", "VPP"};

  void not_root_group_error(const netCDF::NcGroup &group) {
    if (!group.isRootGroup()) {
      LogCriticalError("Specification checks must start at root group");
      CRITICAL_ERROR_POEM
    }
  }

  bool check_R1(const netCDF::NcGroup &group) {
    not_root_group_error(group);

    bool compliant = true;
    if (group.getAtts().contains("POEM_SPECIFICATION_VERSION")) {
      std::string version_;
      group.getAtt("POEM_SPECIFICATION_VERSION").getValues(version_);
      auto version = semver::version::parse(version_, false).major();
      if (version != 1) {
        LogWarningError("POEM file version 1 and POEM_SPECIFICATION_VERSION mismatch ({})", version);
        compliant = false;
      }

    } else {
      compliant = false;
    }

    if (!group.getAtts().contains("VESSEL_NAME")) {
      LogWarningError("VESSEL_NAME not found in root group");
      compliant = false;
    }

    if (!compliant) {
      not_compliant_warning(1);
    }

    return compliant;
  }

  bool check_R2(const netCDF::NcGroup &group) { // TODO: verifier

    bool compliant = true;

    // Are we in a POEM group
    if (group.getAtts().contains("POEM_NODE_TYPE")) {

      std::string node_type;
      group.getAtt("POEM_NODE_TYPE").getValues(node_type);

      if (std::find(known_poem_node_types.begin(), known_poem_node_types.end(), node_type) ==
          known_poem_node_types.end()) {
        LogWarningError("In {}, unknown POEM_NODE_TYPE: {}", group.getName(true), node_type);
        compliant = false;
      }

      // POEM group

      for (const auto &group_: group.getGroups()) {
        compliant &= check_R2(group_.second);
      }

      if (node_type == "POLAR") {
        for (const auto &var: group.getVars()) {
          if (var.second.getAtts().contains("POEM_NODE_TYPE")) {
            std::string var_node_type;
            var.second.getAtt("POEM_NODE_TYPE").getValues(var_node_type);
            if (group.getCoordVars().contains(var.first)) {
              // Coordinate Variable // FIXME: introduire POLAR_DIMENSION
              if (var_node_type != "POLAR_DIMENSION") {
                LogWarningError(
                    "Coordinate Variable {} has attribute POEM_NODE_TYPE set to {}, expected is POLAR_DIMENSION",
                    var.first, var_node_type); // FIXME: MANQUANT
                compliant = false;
              }
            } else {
              // PolarTable
              if (var_node_type != "POLAR_TABLE") {
                LogWarningError("Variable {} has attribute POEM_NODE_TYPE set to {}, expected is POLAR_TABLE",
                                var.first, var_node_type); // FIXME: MANQUANT
                compliant = false;
              }
            }
          }
        }
      }

    } else {
      // Not a POEM group

      if (group.isRootGroup()) {
        // A POEM root group MUST have POEM_NODE_TYPE defined! TODO: ajouter dans la rule...
        LogWarningError("POEM File root group MUST always have an attribute POEM_NODE_TYPE");
        compliant = false;
      }

    }

    if (!compliant) {
      not_compliant_warning(2);
    }

    return compliant;
  }

  bool check_R3(const netCDF::NcGroup &group) {

    bool compliant = true;

    if (group.getAtts().contains("POEM_NODE_TYPE")) {
      std::string node_type;
      group.getAtt("POEM_NODE_TYPE").getValues(node_type);
      if (node_type == "POLAR") {
        if (std::find(known_polar_modes.begin(), known_polar_modes.end(), group.getName()) == known_polar_modes.end()) {
          LogWarningError("Group {} has attribute POEM_NODE_TYPE set to POLAR thus its name must be either "
                          "MPPP, HPPP, MVPP, HVPP or VPP. Its current name is {}",
                          group.getName(true), group.getName(false));
        }
//        if (group.getAtts().contains("POLAR_MODE")) {
//          std::string polar_mode;
//          group.getAtt("POLAR_MODE").getValues(polar_mode);
//          if (std::find(known_polar_types.begin(), known_polar_types.end(), polar_mode) == known_polar_types.end()) {
//            LogWarningError("In group {}, POLAR_MODE attibute must be either "
//                            "MPPP, HPPP, MVPP, HVPP or VPP. Found {}", group.getName(true), polar_mode);
//            compliant = false;
//          }
//        } else {
//          LogWarningError("In group {}, POLAR_MODE attribute not found", group.getName(true));
//          compliant = false;
//        }
      }

      for (const auto &group_: group.getGroups()) {
        compliant &= check_R3(group_.second);
      }

    }

    if (!compliant) {
      not_compliant_warning(3);
    }

    return compliant;
  }

  // FIXME: si on trouve un POLAR_SET, on doit trouver au moins un POLAR dedans -> add
  // FIXME: si on trouve un POLAR, on doit trouver au moins un POLAR_TABLE dedans -> add
  // FIXME: on doit trouver au moins un groupe POLAR dans un fichier

  bool check_R4(const netCDF::NcGroup &group) {
    bool compliant = true;

    if (group.getAtts().contains("POEM_NODE_TYPE")) {
      std::string node_type;
      group.getAtt("POEM_NODE_TYPE").getValues(node_type);

      if (node_type == "POLAR") { // Ajouter le fait qu'on ne peut trouver des POLAR_TABLE que dans POLAR
        for (const auto &var: group.getVars()) {
          if (var.second.getAtts().contains("POEM_NODE_TYPE")) {
            if (var.second.getAtts().contains("unit")) {
              std::string unit;
              var.second.getAtt("unit").getValues(unit);
              auto is_known_unit = dunits::UnitsChecker::getInstance().is_valid_unit(unit, true);
              if (!is_known_unit) { // FIXME: check unite manquant dans spec
                LogWarningError(
                    "In group {}, variable {} unit {} is not known by dunits", // TODO: faire lien dans la doc vers dunits
                    group.getName(true),
                    var.second.getName(),
                    unit);
              }
            } else {
              LogWarningError("In group {}, variable {} does not have a unit attribute",
                              group.getName(true), var.second.getName());
              compliant = false;
            }
            if (!var.second.getAtts().contains("description")) {
              LogWarningError("In group {}, variable {} does not have a description attribute",
                              group.getName(true), var.second.getName());
              compliant = false;
            }

          }
        }
      } else {
        for (const auto &group_: group.getGroups()) {
          compliant &= check_R4(group_.second);
        }
      }

    }

    if (!compliant) {
      LogWarningError(
          "----->> Not compliant with V1/R4 (https://dice-poem.readthedocs.io/en/latest/documentation/rules_v1.html#rule-v1-r4)");
    }

    return compliant;
  }

  bool check_R5(const netCDF::NcGroup &group) {
    bool compliant = true;

    if (group.getAtts().contains("POEM_NODE_TYPE")) { // TODO: changer en POEM_NODE_TYPE
      std::string node_type;
      group.getAtt("POEM_NODE_TYPE").getValues(node_type);
      if (node_type == "POLAR") {
        // TODO: verifier que les dims ont bien des Coord Vars associees qui ont POEM_NODE_TYPE a POLAR_DIMENSION -> add
        for (const auto &var: group.getVars()) {
          std::vector<netCDF::NcDim> dims;
          if (var.second.getAtts().contains("POEM_NODE_TYPE")) {
            auto dims_ = var.second.getDims();
            if (dims.empty()) dims = dims_;
            if (dims_ != dims) {
              LogWarningError("In group {}, variables with POEM_NODE_TYPE=POLAR_TABLE "
                              "defined must all have the same dimensions", group.getName(true));
              compliant = false;
            }
          }
        }

      } else {
        for (const auto &group_: group.getGroups()) {
          compliant &= check_R5(group_.second);
        }
      }
    }

    if (!compliant) {
      not_compliant_warning(5);
    }

    return compliant;
  }

  bool check_R6(const netCDF::NcGroup &group) {
    bool compliant = true;

    if (group.getAtts().contains("POEM_NODE_TYPE")) { // TODO: changer en POEM_NODE_TYPE
      std::string node_type;
      group.getAtt("POEM_NODE_TYPE").getValues(node_type);
      if (node_type == "POLAR") {
        for (const auto &coord_var: group.getCoordVars()) {
          auto nc_var = group.getVar(coord_var.first);

          // FIXME: Coord Vars must have POEM_NODE_TYPE set to POLAR_DIMENSION (voir ou mettre dans une regle precedente) -> add
          if (nc_var.getAtts().contains("POEM_NODE_TYPE")) {
            if (nc_var.getDims().size() != 1) {
              LogCriticalError("In {}, Coordinate Variable {} has more than 1 dimension",
                               group.getName(true), nc_var.getName());
              CRITICAL_ERROR_POEM
            }
            std::vector<double> values(nc_var.getDim(0).getSize());
            nc_var.getVar(values.data());

            double prec = values[0] - 1.;
            for (const auto &val: values) {
              if (val < 0 || val <= prec) {
                LogWarningError("In {}, Coordinate Variable {} must have positive stricly increasing values",
                                group.getName(true), nc_var.getName());
                compliant = false;
              }
              prec = val;
            }

            std::string unit;
            nc_var.getAtt("unit").getValues(unit);
            if (unit == "deg") {
              // FIXME: voir si on peut pas demander a dunits de donner les valeurs d'unites d'un certain type
              //  Pour le moment, on dit que la seule valeur pour les angles c'est le deg -> add
              // Angular variable

              if (values.back() > 180.) {
                LogWarningError("In {}, Angular Coordinate Variable {} values must be between 0 and 180. Found {}",
                                group.getName(true), nc_var.getName(), values.back());
                compliant = false;
              }
            }
          }
        }

      } else {
        for (const auto &group_: group.getGroups()) {
          compliant &= check_R6(group_.second);
        }
      }
    }

    if (!compliant) {
      not_compliant_warning(6);
    }

    return compliant;
  }

  // FIXME: est-ce qu'on garde la regle POLAR doit avoir un attribut POLAR_MODE ?
  //  En fait un groupe POLAR est un groupe dans lequel on trouve des POLAR_TABLE et des POLAR_DIMENSION...
  //  ca devrait suffire pour determiner qu'on est dans un POLAR !!

  bool has_coord_var(const netCDF::NcGroup &group, const std::string &name) {
    bool compliant = true;

    std::string node_type;
    group.getAtt("POEM_NODE_TYPE").getValues(node_type);
    if (node_type != "POLAR") {
      LogCriticalError("Group {} must be a POLAR, found a {}", group.getName(true), node_type);
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
                          group.getName(true), name, var_type);
          compliant = false;
        }
      } else {
        LogWarningError("In {}, mandatory Coordinate Variable {} is found, but attribute POEM_NODE_TYPE is not found",
                        group.getName(true), name);
        compliant = false;
      }
      // TODO: verifier qu'on a bien POEM_NODE_TYPE qui est DIMENSION_NODE
    } else {
//      std::string polar_mode;
//      group.getAtt("POLAR_MODE").getValues(polar_mode);
      LogWarningError("In {} ({} mode), Coordinate Variable {} not found",
                      group.getName(true), group.getName(false), name);
      compliant = false;
    }

    return compliant;
  }

  bool has_var(const netCDF::NcGroup &group, const std::string &name, const std::vector<std::string> &dims) {
    bool compliant = true;

    if (group.getVars().contains(name)) {
      if (group.getCoordVars().contains(name)) {
        LogWarningError("In {}, Variable {} should not be a Coordinate Variable",
                        group.getName(true), name);
        compliant = false;
        // FIXME: cette condition n'existe pas dans les specs
      }
      auto nc_var = group.getVar(name);
      if (nc_var.getAtts().contains("POEM_NODE_TYPE")) {
        std::string node_type;
        nc_var.getAtt("POEM_NODE_TYPE").getValues(node_type);
        if (node_type != "POLAR_TABLE") {
          LogWarningError("In {}, mandatory variable {} found but its attribute "
                          "POEM_NODE_TYPE is not POLAR_TABLE. Found {}",
                          group.getName(true), name, node_type);
          compliant = false;
        }
      } else {
        LogWarningError("In {}, mandatory variable {} found but has no attribute POEM_NODE_TYPE",
                        group.getName(true), name);
        compliant = false;
      }

      auto var_dims = nc_var.getDims();
      if (var_dims.size() != dims.size()) {
        LogWarningError("In {}, mandatory variable {} has not the correct number of dimensions. Expected {}, found {}",
                        group.getName(true), name, dims.size(), var_dims.size());
        compliant = false;
      }
      for (size_t i = 0; i < dims.size(); ++i) {
        if (dims[i] != var_dims[i].getName()) {
          LogWarningError("In {}, mandatory variable {} has incorrect dimension {}. Expected {}, found {}",
                          group.getName(true), name, i, dims[i], var_dims[i].getName());
          compliant = false;
        }
      }

      // TODO: verifier qu'on a bien toutes les dims dans le bon ordre et que la variable est a bien un POEM_NODE_TYPE
      //  a POLAR_TABLE
    } else {
//      std::string polar_mode;
//      group.getAtt("POLAR_MODE").getValues(polar_mode);
      LogWarningError("In {} ({} mode), Variable {} not found",
                      group.getName(true), group.getName(false), name);
      compliant = false;
    }

    return compliant;
  }

  bool check_R7(const netCDF::NcGroup &group) {
    bool compliant = true;

    if (group.getAtts().contains("POEM_NODE_TYPE")) {
      std::string node_type;
      group.getAtt("POEM_NODE_TYPE").getValues(node_type);
      if (node_type == "POLAR") {
//        std::string polar_mode;
//        group.getAtt("POLAR_MODE").getValues(polar_mode);
        auto polar_mode = group.getName(false);
        if (polar_mode == "MPPP" || polar_mode == "HPPP") {
          compliant &= has_coord_var(group, "STW_Coord");
          compliant &= has_coord_var(group, "TWS_Coord");
          compliant &= has_coord_var(group, "TWA_Coord");
          compliant &= has_coord_var(group, "WA_Coord");
          compliant &= has_coord_var(group, "Hs_Coord");
          compliant &= has_var(group, "TOTAL_POWER", {"STW_Coord", "TWS_Coord", "TWA_Coord", "WA_Coord", "Hs_Coord"});
          compliant &= has_var(group, "LEEWAY", {"STW_Coord", "TWS_Coord", "TWA_Coord", "WA_Coord", "Hs_Coord"});
          compliant &= has_var(group, "SOLVER_STATUS", {"STW_Coord", "TWS_Coord", "TWA_Coord", "WA_Coord", "Hs_Coord"});
        }
        if (polar_mode == "MVPP" || polar_mode == "HVPP") {
          compliant &= has_coord_var(group, "Power_Coord");
          compliant &= has_coord_var(group, "TWS_Coord");
          compliant &= has_coord_var(group, "TWA_Coord");
          compliant &= has_coord_var(group, "WA_Coord");
          compliant &= has_coord_var(group, "Hs_Coord");
          compliant &= has_var(group, "STW", {"Power_Coord", "TWS_Coord", "TWA_Coord", "WA_Coord", "Hs_Coord"});
          compliant &= has_var(group, "LEEWAY", {"Power_Coord", "TWS_Coord", "TWA_Coord", "WA_Coord", "Hs_Coord"});
          compliant &= has_var(group, "SOLVER_STATUS",
                               {"Power_Coord", "TWS_Coord", "TWA_Coord", "WA_Coord", "Hs_Coord"});
        }
        if (polar_mode == "VPP") {
          compliant &= has_coord_var(group, "TWS_Coord");
          compliant &= has_coord_var(group, "TWA_Coord");
          compliant &= has_coord_var(group, "WA_Coord");
          compliant &= has_coord_var(group, "Hs_Coord");
          compliant &= has_var(group, "STW", {"TWS_Coord", "TWA_Coord", "WA_Coord", "Hs_Coord"});
          compliant &= has_var(group, "LEEWAY", {"TWS_Coord", "TWA_Coord", "WA_Coord", "Hs_Coord"});
          compliant &= has_var(group, "SOLVER_STATUS", {"TWS_Coord", "TWA_Coord", "WA_Coord", "Hs_Coord"});
        }

      } else {
        for (const auto &group_: group.getGroups()) {
          compliant &= check_R7(group_.second);
        }
      }
    }

    if (!compliant) {
      not_compliant_warning(7);
    }

    return compliant;
  }

//  bool check_R8(const netCDF::NcGroup &group) {
//    NIY_POEM
//  }
//
//  bool check_R9(const netCDF::NcGroup &group) {
//    NIY_POEM
//  }
//
//  bool check_R10(const netCDF::NcGroup &group) {
//    NIY_POEM
//  }

}

bool poem::check_v1(const std::string &filename) {

  if (!fs::exists(filename)) {
    LogCriticalError("NetCDF file not found: {}", filename);
    CRITICAL_ERROR_POEM
  }

  netCDF::NcFile root_group(filename, netCDF::NcFile::read);

  bool compliant = true;
  compliant &= v1::check_R1(root_group);
  compliant &= v1::check_R2(root_group);
  compliant &= v1::check_R3(root_group);
  compliant &= v1::check_R4(root_group);
  compliant &= v1::check_R5(root_group);
  compliant &= v1::check_R6(root_group);
  compliant &= v1::check_R7(root_group);
//  compliant &= v1::check_R8(root_group);
//  compliant &= v1::check_R9(root_group);
//  compliant &= v1::check_R10(root_group);

  root_group.close();

  return compliant;
}
// poem::v1
