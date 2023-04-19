//
// Created by frongere on 13/04/23.
//

#ifndef POEM_POLARSET_H
#define POEM_POLARSET_H

#include <memory>

#include "Polar.h"
#include "Attributes.h"

#include "poem/schema/Schema.h"

namespace poem {

  /**
 * Represents a set of polars.
 *
 * This is what we manipulate directly.
 */
  class PolarSet {
   public:
    using PolarMap = std::unordered_map<std::string, std::unique_ptr<PolarBase>>;
//    using Iter = PolarVector::const_iterator;

    PolarSet(const Attributes &attributes, const Schema &schema) :
        m_attributes(attributes),
        m_schema(schema) {

      // TODO: ajouter le schema s'il n'y est pas
      if (!m_attributes.contains("schema")) {
        m_attributes.add_attribute("schema", schema.json_str());
      }

      m_schema.check_attributes(&m_attributes);
    };

    template<typename T, size_t _dim>
    Polar<T, _dim> *New(const std::string &name,
                        const std::string &unit,
                        const std::string &description,
                        type::POEM_TYPES type,
                        std::shared_ptr<DimensionPointSet<_dim>> dimension_point_set) {

      auto polar = std::make_unique<Polar<T, _dim>>(name, unit, description, type, dimension_point_set);

      m_polars_map.insert({name, std::move(polar)});
      auto polar_ptr = m_polars_map[name].get();
      m_schema.check_polar(polar_ptr);

      return static_cast<Polar<T, _dim> *>(polar_ptr);
    }

    int to_netcdf(const std::string &nc_file) const {

      spdlog::info("Writing NetCDF file {}", nc_file);

      constexpr int nc_err = 2;

      try {

        // Create the file. The Replace parameter tells netCDF to overwrite
        // this file, if it already exists.
        netCDF::NcFile dataFile(nc_file, netCDF::NcFile::replace);

        // Writing attributes
        for (const auto &attribute: m_attributes) {
          dataFile.putAtt(attribute.first, attribute.second);
        }

        for (const auto &polar: m_polars_map) {
          polar.second->to_netcdf(dataFile);
        }

      } catch (netCDF::exceptions::NcException &e) {
        std::cerr << e.what() << std::endl;
        return nc_err;
      }

      return 0;
    }

   private:
    Attributes m_attributes;
    PolarMap m_polars_map;

    Schema m_schema;
  };

}  // poem

#endif //POEM_POLARSET_H
