//
// Created by frongere on 30/05/24.
//

#ifndef POEM_RULES_V0_H
#define POEM_RULES_V0_H

#include <string>
#include <netcdf>

#include "poem/exceptions.h"

namespace poem::v0 {

  std::vector<std::string> mandatory_variables() {
    return {"BrakePower", "LEEWAY"};
  }

  std::vector<std::string> understood_variables() {
    return {"conso_t_h", "HEELING"};
  }

  bool check_attribute(const netCDF::NcVar &var,
                       const std::string &att_name,
                       bool verbose,
                       const std::string &att_value = "") {

    if (var.getAtts().contains(att_name)) {
      if (!att_value.empty()) {
        std::string att_value_;
        var.getAtt(att_name).getValues(att_value_);
        if (att_value_ != att_value) {
          if (verbose) {
            spdlog::warn("In Coordinate Variable {}, Attribute {} value must be {} but {} found instead",
                         var.getName(), att_name, att_value, att_value_);
          }
          return false;
        }
      }
    } else {
      if (verbose) {
        spdlog::warn("No attribute named {} on Coordinate Variable {}", var.getName());
      }
      return false;
    }

    return true;
  }

  bool check_mandatory_variable_dimensions(const netCDF::NcVar &var,
                                           const std::vector<netCDF::NcDim> &dims,
                                           bool verbose) {
    if (var.getDimCount() != dims.size()) {
      if (verbose)
        spdlog::warn("Variable {} must have {} dimensions. Found {}",
                     var.getName(), dims.size(), var.getDimCount());
      return false;
    }

    for (int i = 0; i < dims.size(); ++i) {
      if (var.getDim(i) != dims.at(i)) {
        if (verbose)
          spdlog::warn("In Variable {}, expected dimension {} at position {} but found {} instead",
                       var.getName(), dims.at(i).getName(), i, var.getDim(i).getName());
        return false;
      }
    }

    return true;
  }

  bool check_mandatory_variable(const netCDF::NcGroup &group,
                                const std::string &coord_name,
                                const std::string &unit,
                                bool verbose) {

    if (group.getVars().contains(coord_name)) {
      auto var = group.getVar(coord_name);
      if (!check_attribute(var, "unit", verbose, unit)) return false;
      if (!check_attribute(var, "description", verbose)) return false;

    } else {
      if (verbose) spdlog::warn("Mandatory Variable {} not found", coord_name);
      return false;
    }

    return true;
  }

  bool check_coord_var_values_bounds(const std::vector<double> &values, bool verbose) {
    if (values.back() > 180.) {
      if (verbose) spdlog::warn("Angular Coordinate variables must be numbers between 0 and 180");
      return false;
    }
    return true;
  }

  bool check_coord_var_values(const std::vector<double> &values, bool verbose) {
    double val_ref = values.front();
    if (val_ref < 0.) {
      if (verbose) spdlog::warn("Coordinate Variables must be list of positive numbers");
      return false;
    }
    for (size_t i = 1; i < values.size(); ++i) {
      double val = values.at(i);
      if (val <= val_ref) {
        if (verbose) spdlog::warn("Coordinate Variables must be list of strictly increasing numbers");
        return false;
      }
      val_ref = val;
    }
    return true;
  }

  bool check_coord_var(const netCDF::NcGroup &group,
                       const std::string &coord_name,
                       const std::string &unit,
                       std::vector<double> &values,
                       bool verbose) {

    // Get the dimension
    netCDF::NcDim dim;
    size_t dim_size;
    if (group.getDims().contains(coord_name)) {
      dim = group.getDim(coord_name);
      dim_size = dim.getSize();
    } else {
      if (verbose) spdlog::warn("In {}, no dimension named {}", group.getName(), coord_name);
      return false;
    }

    // Get the variable
    netCDF::NcVar var;
    std::vector<double> values_(dim_size);
    if (group.getCoordVars().contains(coord_name)) {
      var = group.getVar(coord_name);
      if (var.getDims().size() != 1) {
        if (verbose) spdlog::warn("Coordinate Variable {} must have only one dimension", var.getName());
        return false;
      }
      if (var.getDim(0) != dim) {
        if (verbose) spdlog::warn("Coordinate Variable {} must have dimension of the same name", var.getName());
        return false;
      }
      var.getVar(values_.data());

    } else {
      if (verbose) spdlog::warn("No dimension named {}", coord_name);
      return false;
    }

    // Check attributes
    if (!check_attribute(var, "unit", verbose, unit)) {
      return false;
    }
    if (!check_attribute(var, "description", verbose)) {
      return false;
    }

    values = values_;

    return true;
  }

  bool check_rules(const std::string &nc_polar, bool verbose = true) {

    bool compliant;

    netCDF::NcFile ncfile(nc_polar, netCDF::NcFile::read);

    /// Rule 1: polar_type attribute
    compliant = true;
    if (ncfile.getAtts().contains("polar_type")) {
      std::string polar_type;
      ncfile.getAtt("polar_type").getValues(polar_type);
      if (polar_type != "ND") {
        if (verbose) spdlog::warn("Attribute polar_type value must be ND but {} found instead", polar_type);
        compliant = false;
      }
    } else {
      if (verbose) spdlog::warn("No attribute named polar_type");
      compliant = false;
    }
    if (!compliant) {
      if (verbose) spdlog::warn("Not compliant with v0/R1");
      return false;
    }

    /// Rule 2: Mandatory dimensions
    compliant = true;
    std::vector<double> STW_kt, TWS_kt, TWA_deg, WA_deg, Hs_m;
    compliant = check_coord_var(ncfile, "STW_kt", "kt", STW_kt, verbose) & compliant;
    compliant = check_coord_var(ncfile, "TWS_kt", "kt", TWS_kt, verbose) & compliant;
    compliant = check_coord_var(ncfile, "TWA_deg", "deg", TWA_deg, verbose) & compliant;
    compliant = check_coord_var(ncfile, "WA_deg", "deg", WA_deg, verbose) & compliant;
    compliant = check_coord_var(ncfile, "Hs_m", "m", Hs_m, verbose) & compliant;

    if (!compliant) {
      if (verbose) spdlog::warn("Not compliant with v0/R2");
      return false;
    }

    /// Rule 3: strictly increasing list of positive numbers
    compliant = true;
    compliant = check_coord_var_values(STW_kt, verbose) & compliant;
    compliant = check_coord_var_values(TWS_kt, verbose) & compliant;
    compliant = check_coord_var_values(TWA_deg, verbose) & compliant;
    compliant = check_coord_var_values(WA_deg, verbose) & compliant;
    compliant = check_coord_var_values(Hs_m, verbose) & compliant;

    if (!compliant) {
      if (verbose) spdlog::warn("Not compliant with v0/R3");
      return false;
    }

    /// Rule 4: Angular Coordinate Variables between 0 and 180 deg
    compliant = true;
    compliant = check_coord_var_values_bounds(STW_kt, verbose) & compliant;
    compliant = check_coord_var_values_bounds(TWS_kt, verbose) & compliant;
    compliant = check_coord_var_values_bounds(TWA_deg, verbose) & compliant;
    compliant = check_coord_var_values_bounds(WA_deg, verbose) & compliant;
    compliant = check_coord_var_values_bounds(Hs_m, verbose) & compliant;

    if (!compliant) {
      if (verbose) spdlog::warn("Not compliant with v0/R4");
      return false;
    }

    /// Rule 5: Mandatory variables
    compliant = true;
    compliant = check_mandatory_variable(ncfile, "BrakePower", "kW", verbose) & compliant;
    compliant = check_mandatory_variable(ncfile, "LEEWAY", "deg", verbose) & compliant;

    if (!compliant) {
      if (verbose) spdlog::warn("Not compliant with v0/R5");
      return false;
    }

    /// Rule 6: Variables dimensions
    compliant = true;
    std::vector<netCDF::NcDim> dims;
    dims.reserve(5);
    dims.push_back(ncfile.getDim("STW_kt"));
    dims.push_back(ncfile.getDim("TWS_kt"));
    dims.push_back(ncfile.getDim("TWA_deg"));
    dims.push_back(ncfile.getDim("WA_deg"));
    dims.push_back(ncfile.getDim("Hs_m"));

    compliant = check_mandatory_variable_dimensions(ncfile.getVar("BrakePower"), dims, verbose) & compliant;
    compliant = check_mandatory_variable_dimensions(ncfile.getVar("LEEWAY"), dims, verbose) & compliant;

    if (!compliant) {
      if (verbose) spdlog::warn("Not compliant with v0/R6");
      return false;
    }

    /// Rule 7:

    ncfile.close();

    return true;
  }

}  // poem::v0

#endif //POEM_RULES_V0_H
