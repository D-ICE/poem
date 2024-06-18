//
// Created by frongere on 17/05/24.
//

#ifndef POEM_PERFORMANCEPOLARSETREADER_H
#define POEM_PERFORMANCEPOLARSETREADER_H

#include <string>
#include <filesystem>
#include <spdlog/spdlog.h>
#include <netcdf>

#include "specifications/SpecRulesChecker.h"
#include "exceptions.h"

namespace fs = std::filesystem;

namespace poem {

  namespace details {

    template<typename T, size_t _dim>
    inline void read_polar(const netCDF::NcVar &nc_var,
                           const std::unordered_map<std::string, std::shared_ptr<Dimension>> &dimension_map,
                           const std::unordered_map<std::string, std::vector<double>> &dimension_values_map,
                           std::shared_ptr<PolarSet> polar_set,
                           type::POEM_TYPES var_type) {

      std::string hash_name;
      for (int i = 0; i < _dim; ++i) {
        hash_name += nc_var.getDim(i).getName();
      }

      std::unordered_map<std::string, std::shared_ptr<DimensionPointSetBase>> dimension_point_set_map;
      std::shared_ptr<DimensionPointSet<_dim>> dimension_point_set;

      if (dimension_point_set_map.find(hash_name) != dimension_point_set_map.end()) {
        // This dimension point set is already registered, getting it from registry
        dimension_point_set = std::dynamic_pointer_cast<DimensionPointSet<_dim>>(dimension_point_set_map.at(hash_name));

      } else {

        std::array<std::shared_ptr<Dimension>, _dim> array;
        for (int i = 0; i < _dim; ++i) {
          array.at(i) = dimension_map.at(nc_var.getDim(i).getName());
        }
        auto dimension_grid = DimensionGrid(std::make_shared<DimensionSet<_dim>>(array));
        for (int i = 0; i < _dim; ++i) {
          auto dim_name = nc_var.getDim(i).getName();
          dimension_grid.set_values(dim_name, dimension_values_map.at(dim_name));
        }

        dimension_point_set = std::make_shared<DimensionPointSet<_dim>>(dimension_grid);
        dimension_point_set_map.insert({hash_name, dimension_point_set});

      }

      std::string unit, description;
      nc_var.getAtt("unit").getValues(unit);
      nc_var.getAtt("description").getValues(description);

      // Create the polar into the polar set
      auto polar = polar_set->new_polar<T, _dim>(nc_var.getName(),
                                                 unit,
                                                 description,
                                                 var_type,
                                                 dimension_point_set);

      // Get values from nc_var
      auto polar_size = dimension_point_set->size();
      std::vector<T> values(polar_size);
      nc_var.getVar(values.data());

      polar->set_values(values);

    }

    template<size_t _dim>
    inline void read_polar(const netCDF::NcVar &nc_var,
                           const std::unordered_map<std::string, std::shared_ptr<Dimension>> &dimension_map,
                           const std::unordered_map<std::string, std::vector<double>> &dimension_values_map,
                           std::shared_ptr<PolarSet> polar_set) {

      auto type = nc_var.getType();
      switch (type.getId()) {
        case netCDF::NcType::nc_DOUBLE:
          read_polar<double, _dim>(nc_var, dimension_map, dimension_values_map, polar_set, poem::type::DOUBLE);
          return;
        case netCDF::NcType::nc_INT:
          read_polar<int, _dim>(nc_var, dimension_map, dimension_values_map, polar_set, poem::type::INT);
          return;
        default:
          spdlog::critical("Type {} is not managed yet", type.getTypeClass());
          CRITICAL_ERROR_POEM
      }
    }

  } // details

  inline void read_polar(const netCDF::NcVar &nc_var,
                         const std::unordered_map<std::string, std::shared_ptr<Dimension>> &dimension_map,
                         const std::unordered_map<std::string, std::vector<double>> &dimension_values_map,
                         std::shared_ptr<PolarSet> polar_set) {

    size_t nbdim = nc_var.getDimCount();

    switch (nbdim) {
      case 1:
        return details::read_polar<1>(nc_var, dimension_map, dimension_values_map, polar_set);
      case 2:
        return details::read_polar<2>(nc_var, dimension_map, dimension_values_map, polar_set);
      case 3:
        return details::read_polar<3>(nc_var, dimension_map, dimension_values_map, polar_set);
      case 4:
        return details::read_polar<4>(nc_var, dimension_map, dimension_values_map, polar_set);
      case 5:
        return details::read_polar<5>(nc_var, dimension_map, dimension_values_map, polar_set);
      case 6:
        return details::read_polar<6>(nc_var, dimension_map, dimension_values_map, polar_set);
      default:
        spdlog::critical("Polar dimensions lower than 1 or higher than 6 are forbidden");
        CRITICAL_ERROR_POEM
    }
  }

  inline std::shared_ptr<PolarSet> read_polar_set(const netCDF::NcGroup &group) {

    Attributes attributes;
    for (const auto &att: group.getAtts()) {
      std::string att_val;
      att.second.getValues(att_val);
      attributes.add_attribute(att.first, att_val);
    }

    auto polar_set = std::make_shared<PolarSet>(attributes);

    // Get dimensions
    std::unordered_map<std::string, std::shared_ptr<Dimension>> dimension_map;
    std::unordered_map<std::string, std::vector<double>> dimension_values_map;
    for (const auto &dim_: group.getDims()) {
      auto dim_name = dim_.first;

      auto dim_var = group.getVar(dim_name);

      // TODO: on devrait plutot faire un load dynamique...
      std::string unit, description;
      dim_var.getAtt("unit").getValues(unit);
      dim_var.getAtt("description").getValues(description);

      auto dim = std::make_shared<Dimension>(dim_name, unit, description);
      dimension_map.insert({dim_name, dim});

      auto nc_dim = dim_var.getDim(0);
      auto dim_size = nc_dim.getSize();
      std::vector<double> values(dim_size);
      dim_var.getVar(values.data());

      dimension_values_map.insert({dim_name, values});
    }

    // Get polars
    for (const auto &var_: group.getVars()) {
      std::string var_name = var_.first;
      if (dimension_map.find(var_name) != dimension_map.end()) continue;

      read_polar(group.getVar(var_name), dimension_map, dimension_values_map, polar_set);

    }

    return polar_set;
  }


  inline std::shared_ptr<PerformancePolarSet> read_performance_polar_set(const netCDF::NcGroup &group) {
    // Get attributes from the group
    Attributes attributes;
    for (const auto &att: group.getAtts()) {
      std::string att_val;
      att.second.getValues(att_val);
      attributes.add_attribute(att.first, att_val);
    }

    /*
     * Ici, si on trouve pas de version de fichier poem, on considere que c'est v0
     * Si on trouve, on prend la version qui se trouve dans l'attribut poem_file_format_specification_version
     */

    bool has_groups;
    if (attributes.contains("poem_file_format_version")) {
      has_groups = true;
//      NIY_POEM
    } else {
      spdlog::info("POEM file format version v0");
      has_groups = false;
    }

    auto performance_polar_set = std::make_shared<PerformancePolarSet>(attributes);

    // A PolarSet is a NetCDF Dataset, i.e. a group

    if (!has_groups) {
      // For POEM File Format Version v0 only
      // No groups in the NetCDF-4 file, only one DataSet, this is the root group
      performance_polar_set->AddPolarSet(read_polar_set(group));
      NIY_POEM

    } else {



      // TODO: voir si on fixe pas en dur les groupes qu'il est possible d'aller chercher sur la base de PolarType
      for (const auto &group_: group.getGroups()) {
        performance_polar_set->AddPolarSet(read_polar_set(group_.second));
      }
    }

    return performance_polar_set;
  }

  inline std::shared_ptr<PerformancePolarSet> read_performance_polar_set(const std::string &nc_polar) {

    // Does the file exist
    if (!fs::exists(nc_polar)) {
      spdlog::critical("Polar file {} NOT FOUND", nc_polar);
      CRITICAL_ERROR_POEM
    }

    netCDF::NcFile ncfile(nc_polar, netCDF::NcFile::read);
    auto performance_polar_set = read_performance_polar_set(ncfile);
    ncfile.close();
    return performance_polar_set;
  }

}  // poem

#endif //POEM_PERFORMANCEPOLARSETREADER_H
