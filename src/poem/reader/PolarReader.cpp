//
// Created by frongere on 19/04/23.
//

#include <filesystem>
#include <netcdf>

#include "PolarReader.h"


namespace poem {

  void PolarReader::Read(const std::string &nc_polar) {

    m_polar_set = std::make_shared<PolarSet>();

    // Does the file exist
    if (!fs::exists(nc_polar)) {
      spdlog::critical("Polar file {} NOT FOUND", nc_polar);
      CRITICAL_ERROR_POEM
    }

    spdlog::info("Reading polar file {}", nc_polar);
    m_data_file = std::make_unique<netCDF::NcFile>(nc_polar, netCDF::NcFile::read);

    load_attributes();
    load_dimensions();
    load_variables();

  }


  void PolarReader::load_attributes() {
    auto atts_ = m_data_file->getAtts();
    m_attributes = std::make_shared<Attributes>();
    for (const auto &att_: atts_) {
      auto att_name = att_.first;
      std::string att_val;
      att_.second.getValues(att_val);
      m_attributes->add_attribute(att_name, att_val);
    }
  }


  void PolarReader::load_dimensions() {

    m_dimension_map.clear();
    m_dimension_values_map.clear();

    for (const auto &dim_: m_data_file->getDims()) {
      auto dim_name = dim_.first;

      auto dim_var = m_data_file->getVar(dim_name);

      // TODO: on devrait plutot faire un load dynamique...
      std::string unit, description;
      dim_var.getAtt("unit").getValues(unit);
      dim_var.getAtt("description").getValues(description);

      auto dim = std::make_shared<Dimension>(dim_name, unit, description);
      m_dimension_map.insert({dim_name, dim});

      auto nc_dim = dim_var.getDim(0);
      auto dim_size = nc_dim.getSize();
      std::vector<double> values(dim_size);
      dim_var.getVar(values.data());

      m_dimension_values_map.insert({dim_name, values});

    }

  }

  void PolarReader::load_variables() {

    auto vars_ = m_data_file->getVars();
    for (const auto &var_: vars_) {
      auto var_name = var_.first;

      // If the variable corresponds to a dimension, we do not proceed
      if (m_dimension_map.find(var_name) != m_dimension_map.end()) {
        continue;
      }

      load_variable(var_name);

    }

  }

  void PolarReader::load_variable(const std::string &var_name) {

    auto nc_var = m_data_file->getVar(var_name);
    size_t nbdim = nc_var.getDimCount();

    switch (nbdim) {
      case 1:
        return load_variable < 1 > (var_name);
      case 2:
        return load_variable < 2 > (var_name);
      case 3:
        return load_variable < 3 > (var_name);
      case 4:
        return load_variable < 4 > (var_name);
      case 5:
        return load_variable < 5 > (var_name);
      case 6:
        return load_variable < 6 > (var_name);
      default:
        spdlog::critical("Polar dimensions lower than 1 or higher than 6 are forbidden");
        CRITICAL_ERROR_POEM
    }

  }

}  // poem
