//
// Created by frongere on 11/04/23.
//

#ifndef POEM_POLARREADER_H
#define POEM_POLARREADER_H

#include <iostream>
#include <memory>

#include <spdlog/spdlog.h>

#include "poem/exceptions.h"

#include "poem/polar/Dimensions.h"
#include "poem/polar/Polar.h"
#include "poem/polar/PolarSet.h"


namespace fs = std::filesystem;

namespace netCDF {
  class NcFile;
}

namespace poem {

  // Forward declarations
  class PolarSet;

  class Attributes;

  class Dimension;

  class DimensionPointSetBase;


  class PolarReader {
   public:

    void Read(const std::string &nc_polar);

    std::shared_ptr<PolarSet> polar_set() const { return m_polar_set; }

    std::shared_ptr<Attributes> attributes() const { return m_attributes; }

   private:
    void load_attributes();

    void load_dimensions();

    void load_variables();

    void load_variable(const std::string &var_name);

    template<size_t _dim>
    void load_variable(const std::string &var_name);

    template<typename T, size_t _dim>
    void load_variable(const std::string &var_name, type::POEM_TYPES var_type);


   private:
    std::unique_ptr<netCDF::NcFile> m_data_file;
    std::shared_ptr<Attributes> m_attributes;
    std::map<std::string, std::shared_ptr<Dimension>> m_dimension_map;
    std::map<std::string, std::vector<double>> m_dimension_values_map;
    std::map<std::string, std::shared_ptr<DimensionPointSetBase>> m_dimension_point_set_map;

    std::shared_ptr<PolarSet> m_polar_set;

  };

  template<size_t _dim>
  void PolarReader::load_variable(const std::string &var_name) {
    auto nc_var = m_data_file->getVar(var_name);
    auto type = nc_var.getType();
    switch (type.getId()) {
      case netCDF::NcType::nc_DOUBLE:
        load_variable<double, _dim>(var_name, poem::type::DOUBLE);
        return;
      case netCDF::NcType::nc_INT:
        load_variable<int, _dim>(var_name, poem::type::INT);
        return;
      default:
        spdlog::critical("Type {} is not managed yet", type.getTypeClass());
        CRITICAL_ERROR_POEM
    }
  }

  template<typename T, size_t _dim>
  void PolarReader::load_variable(const std::string &var_name, type::POEM_TYPES var_type) {

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
      dimension_point_set = dimension_grid.dimension_point_set();
      m_dimension_point_set_map.insert({hash_name, dimension_point_set});

    }

    std::string unit, description;
    nc_var.getAtt("unit").getValues(unit);
    nc_var.getAtt("description").getValues(description);

    // FIXME: Ou est-ce qu'on va chercher le PPP ??? Pour le moment c'est code en dur...
//    std::cerr << "Le type de polaire (PPP, VPP, HVPP) n'est pas encode. Ici, on " << std::endl;
    auto polar_type = poem::PPP;

    m_polar_set->new_polar<T, _dim>(var_name, unit, description, var_type, polar_type, dimension_point_set);

  }

}  // poem

#endif //POEM_POLARREADER_H
