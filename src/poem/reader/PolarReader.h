//
// Created by frongere on 11/04/23.
//

#ifndef POEM_POLARREADER_H
#define POEM_POLARREADER_H

#include <iostream>
#include <string>
#include <map>
#include <filesystem>

#include "poem/exceptions.h"
#include "poem/polar/PolarSet.h"

namespace fs = std::filesystem;

namespace netCDF {
  class NcFile;
}

namespace poem {

//  // Forward declaration
//  class PolarSet;
//
////  class Dimension_;
//
//  class DimensionPointSetBase;
//
//  class PolarReader {
//   public:
//    PolarReader() = default;
//
//    std::shared_ptr<PolarSet> Read(const std::string &nc_polar);
//
//   private:
//
//    const Attributes load_attributes() const;
//
//    void load_dimensions();
//
//    void load_variable(const std::string &var_name);
//
//    template<size_t _dim>
//    void load_variable(const std::string &var_name);
//
//    template<typename T, size_t _dim>
//    void load_variable(const std::string &var_name);
//
//    template<type::POEM_TYPES type, size_t _dim>
//    void load_var_data(netCDF::NcVar &nc_var, std::shared_ptr<DimensionPointSet<_dim>> dimension_point_set);
//
//   private:
//
//    std::unique_ptr<netCDF::NcFile> m_dataFile;
//
//    std::shared_ptr<PolarSet> m_polar_set;
//
//    std::map<std::string, std::shared_ptr<Dimension>> m_dimension_map;
//    std::map<std::string, std::shared_ptr<DimensionPointSetBase>> m_dim_ID_set_registry;
//
//  };

}  // poem

#endif //POEM_POLARREADER_H
