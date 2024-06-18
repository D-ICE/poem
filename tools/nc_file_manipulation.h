//
// Created by frongere on 31/05/24.
//

#ifndef POEM_NC_FILE_MANIPULATION_H
#define POEM_NC_FILE_MANIPULATION_H

#include <fstream>
#include <filesystem>

#include <netcdf>
#include <spdlog/spdlog.h>
#include <semver/semver.hpp>
#include <nlohmann/json.hpp>

#include "poem/exceptions.h"

namespace fs = std::filesystem;
using namespace poem;


template<typename T>
void duplicate_var(const netCDF::NcVar &varin,
                   netCDF::NcGroup &groupout,
                   const std::unordered_map<std::string, std::string> &new_varnames_map) {

  size_t size = 1;
  std::vector<netCDF::NcDim> dims;
  dims.reserve(varin.getDimCount());
  for (const auto &dim: varin.getDims()) {

    size *= dim.getSize();

    auto dim_name = dim.getName();
    if (new_varnames_map.contains(dim_name)) {
      dim_name = new_varnames_map.at(dim_name);
    }

    auto dimout = groupout.getDim(dim_name);
    if (dimout.isNull()) {
      dimout = groupout.addDim(dim_name, dim.getSize());
    }
    dims.push_back(dimout);
  }

  auto var_name = varin.getName();
  if (new_varnames_map.contains(var_name)) {
    spdlog::info("RENAME {} -> {}", var_name, new_varnames_map.at(var_name));
    var_name = new_varnames_map.at(var_name);
  }

  std::vector<T> values(size);
  varin.getVar(values.data());
  auto varout = groupout.addVar(var_name, varin.getType(), dims);
  varout.putVar(values.data());

  for (const auto &att: varin.getAtts()) {
    // Only supporting string as attributes
    std::string att_val;
    att.second.getValues(att_val);
    varout.putAtt(att.first, att_val);
  }

}


void duplicate_group(const netCDF::NcGroup &groupin, netCDF::NcGroup &groupout,
                     const std::unordered_map<std::string, std::string> &new_varnames_map,
                     const std::vector<std::string> &kept_variables_names) {

  for (const auto &att: groupin.getAtts()) {
    // Only supporting string as attributes
    std::string att_val;
    att.second.getValues(att_val);
    groupout.putAtt(att.first, att_val);
  }


  std::vector<netCDF::NcVar> kept_variables;
  kept_variables.reserve(kept_variables_names.size());
  for (const auto &kept_varname: kept_variables_names) {
    if (groupin.getVars().contains(kept_varname)) {
      kept_variables.push_back(groupin.getVar(kept_varname));
    } else {
      spdlog::warn("Variable {} not found in group {}", kept_varname, groupin.getName());
    }
  }

  for (const auto& var : groupin.getVars()) {
    if (std::find(kept_variables_names.begin(), kept_variables_names.end(), var.first) == kept_variables_names.end()) {
      spdlog::info("DELETE {}", var.first);
//    } else {
//      spdlog::info("KEEP {}", var.first);
    }
  }

  for (const auto &varin: kept_variables) {

    netCDF::NcType::ncType type = varin.getType().getTypeClass();
    switch (type) {
      case netCDF::NcType::nc_DOUBLE: {
        duplicate_var<double>(varin, groupout, new_varnames_map);
        break;
      }

      case netCDF::NcType::nc_INT: {
        duplicate_var<int>(varin, groupout, new_varnames_map);
        break;
      }

      default:
        spdlog::critical("Only double or int");
        CRITICAL_ERROR_POEM
    }

  }


}


int nc_duplicate(const std::string &nc_file_in,
                 const std::string &nc_file_out,
                 const std::unordered_map<std::string, std::string> &new_varnames_map,
                 const std::vector<std::string> &kept_variables_names) {

  // nc_file_in must exist
  if (!fs::exists(nc_file_in)) {
    spdlog::critical("File not found: {}", nc_file_in);
    CRITICAL_ERROR_POEM
  }

  netCDF::NcFile ncfilein(nc_file_in, netCDF::NcFile::read);

  constexpr int nc_err = 2;

  try {
    netCDF::NcFile ncfileout(nc_file_out, netCDF::NcFile::replace);


    duplicate_group(ncfilein, ncfileout, new_varnames_map, kept_variables_names);

    for (const auto &groupin: ncfilein.getGroups()) {
      auto groupout = ncfileout.addGroup(groupin.first);
      duplicate_group(groupin.second, groupout, new_varnames_map, kept_variables_names);
    }

    ncfileout.close();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return nc_err;
  }

  ncfilein.close();

  return 0;
}

void build_description_file(const std::string &nc_file,
                            const std::string &var_file) {

  // FIXME: les mandatory variables doivent etre recuperees de la norme POEM...

  SpecRulesChecker checker(nc_file, false);
  auto mandatory_variables = checker.mandatory_variables();
  auto understood_variables = checker.understood_variables();

  spdlog::info("Generating description file");

  netCDF::NcFile ncfile(nc_file, netCDF::NcFile::read);

  auto file_version = checker.version();

  nlohmann::json json;

  json["_note1"] = "Do not modify fields other than optional_variables";
  json["_note2"] = "In optional_variables uncomment the variable of interest by removing the leading _";


  json["poem_file_description"]["poem_file"] = fs::path(nc_file).filename().string();
  json["poem_file_description"]["poem_file_specification_version"] = file_version.major();

  for (const auto &mvar: mandatory_variables) {
    if (!ncfile.getVars().contains(mvar)) {
      spdlog::critical("Mandatory variable {} not found", mvar);
      CRITICAL_ERROR_POEM
    }
  }
  json["poem_file_description"]["mandatory_variables"] = mandatory_variables;  // FIXME: check qu'elles y sont...

  std::vector<std::string> coordinate_variables;
  for (const auto &coord_var: ncfile.getCoordVars()) {
    coordinate_variables.push_back(coord_var.first);
  }
  json["poem_file_description"]["coordinate_variables"] = coordinate_variables;

  std::vector<std::string> u_variables;
  for (const auto &uvar: understood_variables) {
    if (ncfile.getVars().contains(uvar)) {
      u_variables.push_back(uvar);
    }
  }
  json["poem_file_description"]["understood_variables"] = u_variables;

  std::vector<std::string> optional_variables;
  for (const auto &var: ncfile.getVars()) {
    if (ncfile.getCoordVars().contains(var.first) ||
        std::find(mandatory_variables.begin(), mandatory_variables.end(), var.first) != mandatory_variables.end() ||
        std::find(understood_variables.begin(), understood_variables.end(), var.first) != understood_variables.end()) {
      continue;
    }
    optional_variables.push_back("_" + var.first);
  }

  json["poem_file_description"]["optional_variables"] = optional_variables;

  std::ofstream file(var_file);
  file << std::setw(2) << json << std::endl;

  spdlog::info("Description file written: {}", var_file);

}

void get_info(const std::string &nc_file) {

  netCDF::NcFile ncfile(nc_file, netCDF::NcFile::read);

  SpecRulesChecker checker(nc_file, false);

  std::cout << "POEM " << std::endl;

  for (const auto &coord: ncfile.getCoordVars()) {
    std::cout << coord.first << std::endl;
  }

  ncfile.close();

}

#endif //POEM_NC_FILE_MANIPULATION_H
