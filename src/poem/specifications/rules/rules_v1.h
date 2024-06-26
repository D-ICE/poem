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

  bool check_rules(const std::string &nc_polar, bool verbose = true) {

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


    NIY_POEM



//    compliant = true;
//    std::vector<double> STW_kt, TWS_kt, TWA_deg, WA_deg, Hs_m;
//    compliant = check_coord_var(ncfile, "STW_kt", "kt", STW_kt, verbose) & compliant;
//    compliant = check_coord_var(ncfile, "TWS_kt", "kt", TWS_kt, verbose) & compliant;
//    compliant = check_coord_var(ncfile, "TWA_deg", "deg", TWA_deg, verbose) & compliant;
//    compliant = check_coord_var(ncfile, "WA_deg", "deg", WA_deg, verbose) & compliant;
//    compliant = check_coord_var(ncfile, "Hs_m", "m", Hs_m, verbose) & compliant;
//
//    if (!compliant) {
//      if (verbose) spdlog::critical("Not compliant with v0/R2");
//      return false;
//    }

//    /**
//     * Rule 3: strictly increasing list of positive numbers
//     */
//    compliant = true;
//    compliant = check_coord_var_values(STW_kt, verbose) & compliant;
//    compliant = check_coord_var_values(TWS_kt, verbose) & compliant;
//    compliant = check_coord_var_values(TWA_deg, verbose) & compliant;
//    compliant = check_coord_var_values(WA_deg, verbose) & compliant;
//    compliant = check_coord_var_values(Hs_m, verbose) & compliant;
//
//    if (!compliant) {
//      if (verbose) spdlog::critical("Not compliant with v0/R3");
//      return false;
//    }
//
//    /**
//     * Rule 4: Angular Coordinate Variables between 0 and 180 deg
//     */
//    compliant = true;
//    compliant = check_coord_var_values_bounds(STW_kt, verbose) & compliant;
//    compliant = check_coord_var_values_bounds(TWS_kt, verbose) & compliant;
//    compliant = check_coord_var_values_bounds(TWA_deg, verbose) & compliant;
//    compliant = check_coord_var_values_bounds(WA_deg, verbose) & compliant;
//    compliant = check_coord_var_values_bounds(Hs_m, verbose) & compliant;
//
//    if (!compliant) {
//      if (verbose) spdlog::critical("Not compliant with v0/R4");
//      return false;
//    }
//
//    /**
//     * Rule 5: Mandatory variables
//     */
//    compliant = true;
//    compliant = check_mandatory_variable(ncfile, "BrakePower", "kW", verbose) & compliant;
//    compliant = check_mandatory_variable(ncfile, "LEEWAY", "deg", verbose) & compliant;
//
//    if (!compliant) {
//      if (verbose) spdlog::critical("Not compliant with v0/R5");
//      return false;
//    }
//
//    /**
//     * Rule 6: Variables dimensions
//     */
//    compliant = true;
//    std::vector<netCDF::NcDim> dims;
//    dims.reserve(5);
//    dims.push_back(ncfile.getDim("STW_kt"));
//    dims.push_back(ncfile.getDim("TWS_kt"));
//    dims.push_back(ncfile.getDim("TWA_deg"));
//    dims.push_back(ncfile.getDim("WA_deg"));
//    dims.push_back(ncfile.getDim("Hs_m"));
//
//    compliant = check_mandatory_variable_dimensions(ncfile.getVar("BrakePower"), dims, verbose) & compliant;
//    compliant = check_mandatory_variable_dimensions(ncfile.getVar("LEEWAY"), dims, verbose) & compliant;
//
//    if (!compliant) {
//      if (verbose) spdlog::critical("Not compliant with v0/R6");
//      return false;
//    }
//
//    /**
//     * Rule 7: Coordinate Variables and Variables must define Attributes unit and description
//     */
//    compliant = true;
//    for (const auto& var : ncfile.getVars()) {
//      if (!check_attribute(var.second, "unit", verbose)) {
//        compliant = false;
//        break;
//      }
//      if (!check_attribute(var.second, "description", verbose)) {
//        compliant = false;
//        break;
//      }
//    }
//
//    if (!compliant) {
//      if (verbose) spdlog::critical("Not compliant with v0/R7");
//      return false;
//    }

    ncfile.close();

//    NIY_POEM

    return true;
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
