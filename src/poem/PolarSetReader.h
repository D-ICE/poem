//
// Created by frongere on 11/04/23.
//

#ifndef POEM_POLARSETREADER_H
#define POEM_POLARSETREADER_H

#include <iostream>
#include <memory>
#include <netcdf>

#include <spdlog/spdlog.h>

#include "exceptions.h"
#include "Dimensions.h"
#include "Polar.h"
#include "PolarSet.h"


namespace fs = std::filesystem;

//namespace netCDF {
//  class NcFile;
//}

namespace poem {

//  // Forward declarations
//  class PolarSet;
//
//  class Attributes;
//
//  class Dimension;
//
//  class DimensionPointSetBase;


  class PolarSetReader {
   public:
    PolarSetReader(const std::string &nc_polar) {
      // Does the file exist
      if (!fs::exists(nc_polar)) {
        spdlog::critical("Polar file {} NOT FOUND", nc_polar);
        CRITICAL_ERROR_POEM
      }
      netCDF::NcFile ncfile(nc_polar, netCDF::NcFile::read);
//      PolarSetReader::PolarSetReader(ncfile, ncfile.getGroup("/"));

    }

    PolarSetReader(const netCDF::NcFile& ncfile, const netCDF::NcGroup &group) {

      Attributes attributes;
      for (const auto att : group.getAtts()) {
        std::string att_val;
        att.second.getValues(att_val);
        attributes.add_attribute(att.first, att_val);
        std::cout << att.first << std::endl;
      }





    }

    void Read(const std::string &nc_polar) {
//      Attributes attributes;
//      m_polar_set = std::make_shared<PolarSet>("", attributes);

      // Does the file exist
      if (!fs::exists(nc_polar)) {
        spdlog::critical("Polar file {} NOT FOUND", nc_polar);
        CRITICAL_ERROR_POEM
      }

//      spdlog::info("Reading polar file {}", nc_polar);
//      m_data_file = std::make_unique<netCDF::NcFile>(nc_polar, netCDF::NcFile::read);
//
//      load_attributes();
//      load_dimensions();
//      load_polars();
    }

    std::shared_ptr<PolarSet> polar_set() const {
      return m_polar_set;
    }

    std::shared_ptr<Attributes> attributes() const {
      return m_attributes;
    }

   private:
    void load_attributes() {
      auto atts_ = m_data_file->getAtts();
      m_attributes = std::make_shared<Attributes>();
      for (const auto &att_: atts_) {
        auto att_name = att_.first;
        std::string att_val;
        att_.second.getValues(att_val);
        m_attributes->add_attribute(att_name, att_val);
      }
    }

    void load_dimensions() {
      m_dimension_map.clear();
      m_dimension_values_map.clear();
      m_dimension_point_set_map.clear();

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

    void load_polars() {
      auto vars_ = m_data_file->getVars();
      for (const auto &var_: vars_) {
        auto var_name = var_.first;
        if (m_dimension_map.find(var_name) != m_dimension_map.end()) continue; // This is a dimension, not a polar

        load_polar(var_name);

      }
    }

    void load_polar(const std::string &var_name) {
      auto nc_var = m_data_file->getVar(var_name);
      size_t nbdim = nc_var.getDimCount();

      switch (nbdim) {
        case 1:
          return load_polar<1>(var_name);
        case 2:
          return load_polar<2>(var_name);
        case 3:
          return load_polar<3>(var_name);
        case 4:
          return load_polar<4>(var_name);
        case 5:
          return load_polar<5>(var_name);
        case 6:
          return load_polar<6>(var_name);
        default:
          spdlog::critical("Polar dimensions lower than 1 or higher than 6 are forbidden");
          CRITICAL_ERROR_POEM
      }
    }

    template<size_t _dim>
    void load_polar(const std::string &var_name);

    template<typename T, size_t _dim>
    void load_polar(const std::string &var_name, type::POEM_TYPES var_type);


   private:
    std::shared_ptr<PolarSet> m_polar_set;


    std::unique_ptr<netCDF::NcFile> m_data_file;

    std::unordered_map<std::string, std::shared_ptr<Dimension>> m_dimension_map;
    std::unordered_map<std::string, std::vector<double>> m_dimension_values_map;
    std::unordered_map<std::string, std::shared_ptr<DimensionPointSetBase>> m_dimension_point_set_map;

    std::shared_ptr<Attributes> m_attributes;

  };


  /**
   * Implentations
   */

  template<size_t _dim>
  void PolarSetReader::load_polar(const std::string &var_name) {
    auto nc_var = m_data_file->getVar(var_name);
    auto type = nc_var.getType();
    switch (type.getId()) {
      case netCDF::NcType::nc_DOUBLE:
        load_polar<double, _dim>(var_name, poem::type::DOUBLE);
        return;
      case netCDF::NcType::nc_INT:
        load_polar<int, _dim>(var_name, poem::type::INT);
        return;
      default:
        spdlog::critical("Type {} is not managed yet", type.getTypeClass());
        CRITICAL_ERROR_POEM
    }

  }

  template<typename T, size_t _dim>
  void PolarSetReader::load_polar(const std::string &var_name, type::POEM_TYPES var_type) {

    auto nc_var = m_data_file->getVar(var_name);

    std::string hash_name;
    for (int i = 0; i < _dim; ++i) {
      hash_name += nc_var.getDim(i).getName();
    }

    std::shared_ptr<DimensionPointSet<_dim>> dimension_point_set;

    if (m_dimension_point_set_map.find(hash_name) != m_dimension_point_set_map.end()) {
      // This dimension point set is already registered, getting it from registry
      dimension_point_set = std::dynamic_pointer_cast<DimensionPointSet<_dim>>(m_dimension_point_set_map.at(hash_name));

    } else {

      std::array<std::shared_ptr<Dimension>, _dim> array;
      for (int i = 0; i < _dim; ++i) {
        array.at(i) = m_dimension_map.at(nc_var.getDim(i).getName());
      }
      auto dimension_grid = DimensionGrid(std::make_shared<DimensionSet<_dim>>(array));
      for (int i = 0; i < _dim; ++i) {
        auto dim_name = nc_var.getDim(i).getName();
        dimension_grid.set_values(dim_name, m_dimension_values_map.at(dim_name));
      }

      dimension_point_set = std::make_shared<DimensionPointSet<_dim>>(dimension_grid);
      m_dimension_point_set_map.insert({hash_name, dimension_point_set});

    }

    std::string unit, description;
    nc_var.getAtt("unit").getValues(unit);
    nc_var.getAtt("description").getValues(description);

    // FIXME: Ou est-ce qu'on va chercher le PPP ??? Pour le moment c'est code en dur...
    auto polar_type = poem::PPP;

    // Create the polar into the polar set
    auto polar = m_polar_set->new_polar<T, _dim>(var_name, unit, description,
                                                 var_type, polar_type,
                                                 dimension_point_set);

    // Get values from nc_var
    auto polar_size = dimension_point_set->size();
    std::vector<T> values(polar_size);
    nc_var.getVar(values.data());

    polar->set_values(values);

  }

}  // poem

#endif //POEM_POLARSETREADER_H
