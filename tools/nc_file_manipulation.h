//
// Created by frongere on 31/05/24.
//

#ifndef POEM_NC_FILE_MANIPULATION_H
#define POEM_NC_FILE_MANIPULATION_H

#include <fstream>
#include <netcdf>
#include <spdlog/spdlog.h>
#include <semver/semver.hpp>
#include <nlohmann/json.hpp>

#include "poem/exceptions.h"

using namespace poem;

void rename_variable(const std::string &nc_file,
                     const std::string &old_var_name,
                     const std::string &new_var_name) {

  netCDF::NcFile ncfile(nc_file, netCDF::NcFile::write);

  ncfile.redef();

  spdlog::info("Renaming variable {} into {}", old_var_name, new_var_name);

  if (!ncfile.getVars().contains(old_var_name)) {
    spdlog::critical("Variable {} not found", old_var_name);
    CRITICAL_ERROR_POEM
  }

  if (ncfile.getCoordVars().contains(old_var_name)) {
    // We must also change the name of the corresponding dimension
    spdlog::critical("Variable to be changed is a coordinate variable. Renaming is forbidden.");

  } else {
    auto var = ncfile.getVar(old_var_name);
    var.rename(new_var_name);
  }

  ncfile.close();

}

void get_variable_file(const std::string &nc_file,
                       const std::string &var_file,
                       const std::vector<std::string> &mandatory_variables,
                       const std::vector<std::string> &understood_variables) {

  spdlog::info("Generating description file");

  netCDF::NcFile ncfile(nc_file, netCDF::NcFile::read);

  // Get the file version
  std::string file_poem_version_str;
  if (ncfile.getAtts().contains("poem_file_format_version")) {
    ncfile.getAtt("poem_file_format_version").getValues(file_poem_version_str);
  } else {
    // poem_file_format_version was not a mandatory attribute
    file_poem_version_str = "v0";
  }
  auto file_version = semver::version::parse(file_poem_version_str, false);

  nlohmann::json json;

  json["_note1"] = "Do not modify other fields than optional_variables";
  json["_note2"] = "In optional_variables uncomment the variable of interest by removing the leading _";


  json["poem_file_description"]["poem_file"] = fs::path(nc_file).filename().string();
  json["poem_file_description"]["poem_file_version"] = file_version.major();

  for (const auto& mvar : mandatory_variables) {
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
  for (const auto& uvar : understood_variables) {
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

  for (const auto &coord: ncfile.getCoordVars()) {
    std::cout << coord.first << std::endl;

  }

  ncfile.close();

}

#endif //POEM_NC_FILE_MANIPULATION_H
