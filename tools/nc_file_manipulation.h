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
using json = nlohmann::json;


class VariablesCleaner {

 public:
  enum MODE {
    KEEP_EVERYTHING,   // Keep everything (description file is not used)
    REMOVE_EXPLICIT,   // Remove only those variables explicitly commented in the description file
    REMOVE_ALL_EXCEPT  // Remove every optional variable unless they are present, uncommented, in the description file
  };

 public:
  VariablesCleaner() : m_mode(KEEP_EVERYTHING), m_poem_file_version(0) {}

  VariablesCleaner(const std::string &description_file, MODE mode) : m_mode(mode) {
    std::ifstream ifs(description_file);
    json node = json::parse(ifs);

    if (!node.contains("poem_file_description")) {
      spdlog::critical("In description file, key poem_file_description not found.");
      CRITICAL_ERROR_POEM
    }

    node = node["poem_file_description"];

    m_poem_file_version = node["poem_file_version"].get<int>();

    auto coordinate_variables = node["coordinate_variables"].get<std::vector<std::string>>();
    auto mandatory_variables = node["mandatory_variables"].get<std::vector<std::string>>();
    auto optional_variables = node["optional_variables"].get<std::vector<std::string>>();

    SpecRules rules(m_poem_file_version);

    // Check that coordinate_variables and mandatory variables are present
    for (const auto &coord_var: rules.coordinate_variables()) {
      if (std::find(coordinate_variables.begin(), coordinate_variables.end(), coord_var) ==
          coordinate_variables.end()) {
        spdlog::critical("For poem file version {}, coordinate variable {} is mandatory",
                         m_poem_file_version, coord_var);
        CRITICAL_ERROR_POEM
      }
      m_kept_variables.push_back(coord_var);
    }
    for (const auto &mandatory_var: rules.mandatory_variables()) {
      if (std::find(mandatory_variables.begin(), mandatory_variables.end(), mandatory_var) ==
          mandatory_variables.end()) {
        spdlog::critical("For poem file version {}, variable {} is mandatory",
                         m_poem_file_version, mandatory_var);
        CRITICAL_ERROR_POEM
      }
      m_kept_variables.push_back(mandatory_var);
    }

    for (const auto &var: optional_variables) {
      if (var.starts_with('_')) {
        m_explicitly_removed.push_back(var.substr(1, var.size()));
      } else {
        m_kept_variables.push_back(var);
      }
    }

  }

  [[nodiscard]] bool keep(const std::string &varname) const {
    bool keep;
    switch (m_mode) {
      case KEEP_EVERYTHING:
        keep = true;
        break;
      case REMOVE_EXPLICIT: {
        // We remove only if the variable has been explicitly commented in the description file
        if (std::find(m_explicitly_removed.begin(), m_explicitly_removed.end(), varname) !=
            m_explicitly_removed.end()) {
          keep = false;
        } else {
          keep = true;
        }
        break;
      }
      case REMOVE_ALL_EXCEPT: {
        if (std::find(m_kept_variables.begin(), m_kept_variables.end(), varname) !=
            m_kept_variables.end()) {
          keep = true;
        } else {
          keep = false;
        }
        break;
      }
    }
    return keep;
  }

  [[nodiscard]] int poem_file_version() const { return m_poem_file_version; }

 private:

  int m_poem_file_version;
  MODE m_mode;
  std::vector<std::string> m_kept_variables;
  std::vector<std::string> m_explicitly_removed;

};

int get_spec_version_from_file(const std::string &nc_file) {
  netCDF::NcFile ncfile(nc_file, netCDF::NcFile::read);
  std::string file_poem_version_str;
  if (ncfile.getAtts().contains("poem_file_format_version")) {
    ncfile.getAtt("poem_file_format_version").getValues(file_poem_version_str);
  } else {
    // poem_file_format_version was first introduced in v1
    file_poem_version_str = "v0";
  }
  ncfile.close();
  auto poem_file_version = semver::version::parse(file_poem_version_str, false);
  return poem_file_version.major();
}

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
  varout.setCompression(true, true, 5);

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
                     const VariablesCleaner &cleaner) {

  for (const auto &att: groupin.getAtts()) {
    // Only supporting string as attributes
    std::string att_val;
    att.second.getValues(att_val);
    groupout.putAtt(att.first, att_val);
  }

  std::vector<netCDF::NcVar> kept_variables;
  for (const auto &var: groupin.getVars()) {
    if (cleaner.keep(var.first)) {
      kept_variables.push_back(var.second);
    } else {
      spdlog::info("DELETE variable: {}", var.first);
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
                 const VariablesCleaner &cleaner) {

  // nc_file_in must exist
  if (!fs::exists(nc_file_in)) {
    spdlog::critical("File not found: {}", nc_file_in);
    CRITICAL_ERROR_POEM
  }

  netCDF::NcFile ncfilein(nc_file_in, netCDF::NcFile::read);

  constexpr int nc_err = 2;

  try {
    netCDF::NcFile ncfileout(nc_file_out, netCDF::NcFile::replace);


    duplicate_group(ncfilein, ncfileout, new_varnames_map, cleaner);

    for (const auto &groupin: ncfilein.getGroups()) {
      auto groupout = ncfileout.addGroup(groupin.first);
      duplicate_group(groupin.second, groupout, new_varnames_map, cleaner);
    }

    ncfileout.close();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return nc_err;
  }

  ncfilein.close();

  return 0;
}

int nc_duplicate(const std::string &nc_file_in,
                 const std::string &nc_file_out,
                 const std::unordered_map<std::string, std::string> &new_varnames_map) {
  VariablesCleaner cleaner;
  return nc_duplicate(nc_file_in, nc_file_out, new_varnames_map, cleaner);
}

void build_description_file(const std::string &nc_file,
                            const std::string &var_file) {

  // FIXME: les mandatory variables doivent etre recuperees de la norme POEM...

//  SpecRulesChecker checker(nc_file, false);
  SpecRules spec_rules(get_spec_version_from_file(nc_file));
//  NIY_POEM

  auto mandatory_variables = spec_rules.mandatory_variables();
  auto understood_variables = spec_rules.understood_variables();

  spdlog::info("Generating description file");

  netCDF::NcFile ncfile(nc_file, netCDF::NcFile::read);

  auto file_version = spec_rules.version();

  nlohmann::json json;

  json["_note1"] = "Do not modify fields other than optional_variables";
  json["_note2"] = "In optional_variables uncomment the variable of interest by removing the leading _";


  json["poem_file_description"]["poem_file"] = fs::path(nc_file).filename().string();
  json["poem_file_description"]["poem_file_specification_version"] = file_version;

  for (const auto &mvar: mandatory_variables) {
    if (!ncfile.getVars().contains(mvar)) {
      spdlog::critical("Mandatory variable {} not found", mvar);
      CRITICAL_ERROR_POEM
    }
  }
  json["poem_file_description"]["mandatory_variables"] = mandatory_variables;  // FIXME: check qu'elles y sont...

  std::vector<std::string> coordinate_variables;
  for (const auto &cvar: ncfile.getCoordVars()) {
    coordinate_variables.push_back(cvar.first);
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

  spdlog::info("Get global information on the polar file: {}", nc_file);
  netCDF::NcFile ncfile(nc_file, netCDF::NcFile::read);

//  SpecRulesChecker checker(nc_file, false);
  SpecRules spec_rules(get_spec_version_from_file(nc_file));

  auto one_var = ncfile.getVar(spec_rules.mandatory_variables().back());
  std::cout << "Dimensions [" << one_var.getDims().size() << "]" << std::endl;
  size_t n = 1;
  for (const auto &dim: one_var.getDims()) {
    std::cout << "\t* " << dim.getName() << " [" << dim.getSize() << "]" << std::endl;
    n *= dim.getSize();
  }
  std::cout << "Number of polar points: " << n << std::endl;

  std::cout << "Variables [" << ncfile.getVars().size() << "]" << std::endl;
  for (const auto &var: ncfile.getVars()) {
    if (ncfile.getCoordVars().contains(var.first)) continue;
    std::cout << "\t* " << var.first << std::endl;
  }

  ncfile.close();
  spdlog::info("End informations");
}

#endif //POEM_NC_FILE_MANIPULATION_H
