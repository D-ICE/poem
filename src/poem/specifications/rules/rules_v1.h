//
// Created by frongere on 26/06/24.
//

#ifndef POEM_RULES_V1_H
#define POEM_RULES_V1_H

#include <string>
#include <netcdf>

#include "poem/exceptions.h"
#include "poem/specifications/SpecRules.h"
#include "poem/specifications/SpecRulesBase.h"

/**
 * =====================================================================================================================
 * POEM Specification rules v1
 * =====================================================================================================================
 *
 * >>>>>>>>>>>>>>>   PLEASE DO NOT MODIFY THIS FILE   <<<<<<<<<<<<<<<<
 *
 * As this file describes a specification, any modification MUST give rise to a new specification number.
 * The only modification allowed to this file MIGHT be to fix bugs, not to modify rules
 *
 */

namespace poem::v1 {

  inline bool check_attribute(const netCDF::NcVar &var,
                       const std::string &att_name,
                       bool verbose,
                       const std::string &att_value = "") {
    bool compliant = true;

    if (var.getAtts().contains(att_name)) {
      if (!att_value.empty()) {
        std::string att_value_;
        var.getAtt(att_name).getValues(att_value_);
        if (att_value_ != att_value) {
          if (verbose) {
            spdlog::critical("In Coordinate Variable {}, Attribute {} value must be {} but {} found instead",
                             var.getName(), att_name, att_value, att_value_);
          }
          compliant = false;
        }
      }
    } else {
      if (verbose) {
        spdlog::critical("No attribute named {} on Variable {}", att_name, var.getName());
      }
      compliant = false;
    }

    return compliant;
  }

  inline bool check_R8(const netCDF::NcGroup &group, bool verbose) {
    bool compliant = true;

    for (const auto& var : group.getVars()) {
      compliant = check_attribute(var.second, "unit", verbose) & compliant;
      compliant = check_attribute(var.second, "description", verbose) & compliant;
    }

    return compliant;
  }


  inline bool check_dim(const netCDF::NcVar &var, const std::vector<std::string> &expected_dims, bool verbose) {
    bool compliant = true;

    auto group_name = var.getParentGroup().getName();

    if (var.getDimCount() != expected_dims.size()) {
      if (verbose) spdlog::critical("{} polar MUST have 5 dimensions", group_name);
      compliant = false;
    }

    size_t i = 0;
    for (const auto &expected_dim_name: expected_dims) {
      auto dim_name = var.getDim(i).getName();
      if (dim_name != expected_dim_name) {
        if (verbose)
          spdlog::critical("Dimension {} of {} polar MUST be {}. {} found instead.",
                           i, group_name, expected_dim_name, dim_name);
      }

      i++;
    }

    return compliant;
  }

  inline bool check_R7(const netCDF::NcGroup &group, bool verbose) {
    bool compliant = true;

    auto group_name = group.getName();
    if (group_name == "PPP" || group_name == "HPPP") {
      auto PB = group.getVar("TotalBrakePower");
      compliant = check_dim(PB, {"STW_Coord", "TWS_Coord", "TWA_Coord", "WA_Coord", "Hs_Coord"}, verbose) & compliant;
    }
    if (group_name == "HVPP-PB" || group_name == "VPP-PB") {
      auto STW = group.getVar("STW");
      compliant = check_dim(STW, {"PB_Coord", "TWS_Coord", "TWA_Coord", "WA_Coord", "Hs_Coord"}, verbose) & compliant;
    }

    if (group_name == "VPP") {
      auto LEEWAY = group.getVar("LEEWAY");
      compliant = check_dim(LEEWAY, {"TWS_Coord", "TWA_Coord", "WA_Coord", "Hs_Coord"}, verbose) & compliant;
    }

    return compliant;
  }


  inline bool check_R6(const netCDF::NcGroup &group, bool verbose) {
    bool compliant = true;
    auto group_name = group.getName();


    if (group_name == "PPP" || group_name == "HPPP") {
      if (!group.getVars().contains("TotalBrakePower")) {
        if (verbose) spdlog::critical("In group {}, mandatory variable {} not found.", group_name, "TotalBrakePower");
        compliant = false;
      }
    }

    if (group_name == "HVPP-PB" || group_name == "VPP-PB" || group_name == "VPP") {
      if (!group.getVars().contains("STW")) {
        if (verbose) spdlog::critical("In group {}, mandatory variable {} not found.", group_name, "STW");
        compliant = false;
      }
    }

    if (!group.getVars().contains("LEEWAY")) {
      if (verbose) spdlog::critical("In group {}, mandatory variable {} not found.", group_name, "LEEWAY");
      compliant = false;
    }

    return compliant;
  }

  inline bool check_R4_R5(const netCDF::NcGroup &group, bool verbose) {

    bool compliant = true;

    for (const auto &var: group.getCoordVars()) {
      auto var_ = group.getVar(var.first);
      auto dim = var_.getDim(0);
      std::vector<double> values(dim.getSize());
      var_.getVar(values.data());

      double val_ref = values.front();
      if (val_ref < 0.) {
        if (verbose)
          spdlog::critical("Negative values in coordinate variable {} in group {}",
                           var.first, group.getName());
        compliant = false;
      }
      for (size_t i = 1; i < values.size(); ++i) {
        double val = values.at(i);
        if (val <= val_ref) {
          if (verbose) spdlog::critical("Coordinate Variables must be list of strictly increasing numbers");
          compliant = false;
        }
        val_ref = val;
      }

      if (var.first == "TWA_Coord" || var.first == "WA_Coord") {
        if (values.back() > 180.) {
          if (verbose)
            spdlog::critical("Angular coordinate {} MUST be defined between [0 and 180]. Value found: {}",
                             var.first, values.back());
          compliant = false;
        }
      }

    }
    return compliant;
  }


  inline bool check_dim_existence(const netCDF::NcGroup &group,
                           const std::vector<std::string> &expected_dims,
                           bool verbose) {
    bool compliant = true;
    for (const auto &dim: expected_dims) {
      if (!group.getDims().contains(dim)) {
        compliant = false;
        if (verbose) spdlog::critical("In group {}, mandatory dimension {} not found", group.getName(), dim);
      }
    }
    return compliant;
  }

  inline bool check_rules(const std::string &nc_polar, bool verbose = true) {

    bool compliant;

    netCDF::NcFile ncfile(nc_polar, netCDF::NcFile::read);

    /**
     * Rule 1: global attributes
     */
    compliant = true;
    if (ncfile.getAtts().contains("file_type")) {
      std::string polar_type;
      ncfile.getAtt("file_type").getValues(polar_type);
      if (polar_type != "poem") {
        if (verbose) spdlog::critical("Attribute polar_type value must be poem but {} found instead", polar_type);
        compliant = false;
      }
    } else {
      if (verbose) spdlog::critical("No attribute named file_type");
      compliant = false;
    }
    if (ncfile.getAtts().contains("poem_file_format_version")) {
      std::string poem_file_format_version;
      ncfile.getAtt("poem_file_format_version").getValues(poem_file_format_version);
      if (poem_file_format_version != "v1") {
        if (verbose)
          spdlog::critical("Attribute poem_file_format_version value must be v1 but {} found instead",
                           poem_file_format_version);
        compliant = false;
      }
    } else {
      if (verbose) spdlog::critical("No attribute poem_file_format_version");
      compliant = false;
    }


    if (!compliant) {
      if (verbose) spdlog::critical("Not compliant with v1/R1");
      return false;
    }

    /**
     * Rule 2: Groups
     */
    std::vector<std::string> known_groups = {"PPP", "HPPP", "HVPP-PB", "VPP-PB", "VPP"};
    compliant = true;
    if (ncfile.getGroups().empty()) {
      spdlog::critical("POEM File format Specification v1 only works with groups. No groups found.");
      compliant = false;
    } else {
      for (const auto &group: ncfile.getGroups()) {
        std::string group_name = group.first;
        if (std::find(known_groups.begin(), known_groups.end(), group_name) == known_groups.end()) {
          spdlog::critical("Group name {} is not known by the POEM File Format Specification v1", group_name);
          compliant = false;
          break;
        }
      }
    }

    // if HPPP exists, PPP must exist too
    if (ncfile.getGroups().contains("HPPP") && !ncfile.getGroups().contains("PPP")) {
      spdlog::critical("When HPPP group exists, PPP group must also exist");
      compliant = false;
    }

    // if HPPP exists, PPP must exist too
    if (ncfile.getGroups().contains("HVPP-PB") && !ncfile.getGroups().contains("VPP-PB")) {
      spdlog::critical("When HVPP-PB group exists, VPP-PB group must also exist");
      compliant = false;
    }

    if (!compliant) {
      if (verbose) spdlog::critical("Not compliant with v1/R2");
      return false;
    }

    /**
     * Rule 3: Groups
     */
    compliant = true;
    if (ncfile.getGroups().contains("PPP")) {
      compliant = check_dim_existence(ncfile.getGroup("PPP"),
                                      {"STW_Coord", "TWS_Coord", "TWA_Coord", "WA_Coord", "Hs_Coord"},
                                      verbose) & compliant;
    }
    if (ncfile.getGroups().contains("HPPP")) {
      compliant = check_dim_existence(ncfile.getGroup("HPPP"),
                                      {"STW_Coord", "TWS_Coord", "TWA_Coord", "WA_Coord", "Hs_Coord"},
                                      verbose) & compliant;
    }
    if (ncfile.getGroups().contains("HVPP-PB")) {
      compliant = check_dim_existence(ncfile.getGroup("HVPP-PB"),
                                      {"PB_Coord", "TWS_Coord", "TWA_Coord", "WA_Coord", "Hs_Coord"},
                                      verbose) & compliant;
    }
    if (ncfile.getGroups().contains("VPP-PB")) {
      compliant = check_dim_existence(ncfile.getGroup("VPP-PB"),
                                      {"PB_Coord", "TWS_Coord", "TWA_Coord", "WA_Coord", "Hs_Coord"},
                                      verbose) & compliant;
    }
    if (ncfile.getGroups().contains("VPP")) {
      compliant = check_dim_existence(ncfile.getGroup("VPP"),
                                      {"TWS_Coord", "TWA_Coord", "WA_Coord", "Hs_Coord"},
                                      verbose) & compliant;
    }

    if (!compliant) {
      if (verbose) spdlog::critical("Not compliant with v1/R3");
      return false;
    }

    /**
     * Rule 4 & 5: strictly increasing list of positive numbers. Angular variables between 0 and 180 deg.
     */
    compliant = true;
    for (const auto &group: ncfile.getGroups()) {
      compliant = check_R4_R5(group.second, verbose) & compliant;
    }
    if (!compliant) {
      if (verbose) spdlog::critical("Not compliant with v0/R4 or v0/R5");
      return false;
    }

    /**
     * Rule 6: Mandatory variables
     */
    compliant = true;
    for (const auto &group: ncfile.getGroups()) {
      compliant = check_R6(group.second, verbose) & compliant;
    }
    if (!compliant) {
      if (verbose) spdlog::critical("Not compliant with v0/R6");
      return false;
    }

    /**
     * Rule 7: Order of dimensions
     */
    compliant = true;
    for (const auto &group: ncfile.getGroups()) {
      compliant = check_R7(group.second, verbose) & compliant;
    }
    if (!compliant) {
      if (verbose) spdlog::critical("Not compliant with v0/R7");
      return false;
    }

    /**
     * Rule 8: Mandatory attributes on variables
     */
    compliant = true;
    for (const auto &group: ncfile.getGroups()) {
      compliant = check_R8(group.second, verbose) & compliant;
    }
    if (!compliant) {
      if (verbose) spdlog::critical("Not compliant with v0/R8");
      compliant = false;
    }

    ncfile.close();

    return compliant;
  }


  class SpecRules : public SpecRulesBase {
   public:
    explicit SpecRules() : SpecRulesBase(1) {}

    std::vector<std::string> coordinate_variables() const override {
      return {"STW_kt", "TWS_kt", "TWA_deg", "WA_deg", "Hs_m"};
    }

    std::vector<std::string> mandatory_variables() const override {
      return {"BrakePower", "LEEWAY"};
    }

    std::vector<std::string> understood_variables() const override {
      return {"conso_t_h", "HEELING"};
    }

    bool check(const std::string &nc_polar_file, bool verbose) const override {
      return check_rules(nc_polar_file, verbose);
    }

  };

}  // poem::v1

#endif //POEM_RULES_V1_H
