//
// Created by frongere on 13/04/23.
//

#ifndef POEM_POLARSET_H
#define POEM_POLARSET_H

#include <memory>

#include "Polar.h"
#include "Attributes.h"

namespace poem {

  /**
 * Represents a set of polars.
 *
 * This is what we manipulate directly.
 */
  class PolarSet {
   public:
    using PolarVector = std::vector<std::unique_ptr<PolarBase>>;
    using Iter = PolarVector::const_iterator;

    explicit PolarSet(const Attributes& attributes) : m_attributes(attributes) {};

    template<typename T, size_t _dim>
    Polar<T, _dim> *New(const std::string &name,
                        const std::string &unit,
                        const std::string &description,
                        type::POEM_TYPES type,
                        std::shared_ptr<DimensionPointSet<_dim>> dimension_point_set) {

      auto polar = std::make_unique<Polar<T, _dim>>(name, unit, description, type, dimension_point_set);
      m_polars.push_back(std::move(polar));
      return static_cast<Polar<T, _dim> *>(m_polars.back().get());
    }

    Iter begin() const {
      return m_polars.cbegin();
    }

    Iter end() const {
      return m_polars.cend();
    }

    int to_netcdf(const std::string &nc_file) const {

      spdlog::info("Writing NetCDF file {}", nc_file);

      constexpr int nc_err = 2;

      try {

        // Create the file. The Replace parameter tells netCDF to overwrite
        // this file, if it already exists.
        netCDF::NcFile dataFile(nc_file, netCDF::NcFile::replace);

        // Writing attributes
        for (const auto& attribute : m_attributes) {
          dataFile.putAtt(attribute.first, attribute.second);
        }

        for (const auto &polar: m_polars) {
          polar->to_netcdf(dataFile);
        }

        dataFile.getName() = "coucou";

      } catch (netCDF::exceptions::NcException &e) {
        std::cerr << e.what() << std::endl;
        return nc_err;
      }

      return 0;
    }

   private:
    Attributes m_attributes;
    PolarVector m_polars;
  };

}  // poem

#endif //POEM_POLARSET_H
