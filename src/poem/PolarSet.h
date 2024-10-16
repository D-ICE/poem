//
// Created by frongere on 13/04/23.
//

#ifndef POEM_POLARSET_H
#define POEM_POLARSET_H

#include <memory>
#include <map>
#include <thread>
#include <mutex>
#include <filesystem>

#include "Polar.h"
#include "Attributes.h"
#include "PolarType.h"

namespace fs = std::filesystem;

namespace poem {

  class PolarSet {
   public:
    using PolarMap = std::unordered_map<std::string, std::shared_ptr<PolarBase>>;

    explicit PolarSet(const Attributes &attributes, POLAR_TYPE polar_type) :
        m_attributes(attributes),
        m_polar_type(polar_type) {

      if (attributes["polar_type"] != "ND") {  // ND is for v0 that do not contain groups...
        if (!attributes.contains("name")) {
          spdlog::critical("name attribute is mandatory for PolarSet");
          CRITICAL_ERROR_POEM
        }
      }
    }

    const std::string &name() const { return m_attributes["name"]; }

    POLAR_TYPE polar_type() const {
      return polar_type_s2enum(m_attributes["polar_type"]);
    }

    const std::string &polar_type_str() const {
      return m_attributes["polar_type"];
    }

    template<typename T, size_t _dim>
    std::shared_ptr<Polar<T, _dim>> new_polar(const std::string &name,
                                              const std::string &unit,
                                              const std::string &description,
                                              type::POEM_TYPES type,
                                              std::shared_ptr<DimensionPointSet<_dim>> dimension_point_set) {

      // Thread safety
      std::lock_guard<std::mutex> lock(m_mutex);

      if (m_polars_map.find(name) != m_polars_map.end()) {
        return std::reinterpret_pointer_cast<Polar<T, _dim>>(m_polars_map.at(name));
      }

      auto polar = std::make_shared<Polar<T, _dim>>(name, unit, description, type, m_polar_type, dimension_point_set);

      m_polars_map.insert({name, polar});

      return polar;
    }

    std::shared_ptr<PolarBase> polar(const std::string &name) const {
      try {
        return m_polars_map.at(name);
      } catch (const std::out_of_range &e) {
        spdlog::critical("No polar with name {}", name);
        CRITICAL_ERROR_POEM
      }
    }

    template<typename T, size_t _dim, typename = std::enable_if_t<!std::is_same_v<T, double>>>
    std::shared_ptr<Polar<T, _dim>> polar(const std::string &name) const {
      return std::reinterpret_pointer_cast<Polar<T, _dim>>(polar(name));
    }

    template<typename T, size_t _dim, typename = std::enable_if_t<std::is_same_v<T, double>>>
    std::shared_ptr<InterpolablePolar<_dim>> polar(const std::string &name) const {
      return std::reinterpret_pointer_cast<InterpolablePolar<_dim>>(polar(name));
    }

    std::vector<std::string> polar_names() const {
      std::vector<std::string> polar_names;
      for (const auto &polar: m_polars_map) {
        polar_names.push_back(polar.first);
      }
      return polar_names;
    }

    std::mutex *mutex() {
      return &m_mutex;
    }

    int to_netcdf(netCDF::NcGroup &group) const {

      for (const auto &polar: m_polars_map) {
        polar.second->to_netcdf(group);
      }

      for (const auto &attribute: m_attributes) {
        group.putAtt(attribute.first, attribute.second);
      }

      return 0;
    }

    int to_netcdf(const std::string &nc_file) const {

      fs::path nc_file_path(nc_file);
      if (nc_file_path.is_relative()) {
        nc_file_path = fs::current_path() / nc_file_path;
      }

      spdlog::info("Writing NetCDF file {}", std::string(nc_file_path));

      constexpr int nc_err = 2;
      int code;

      try {

        netCDF::NcFile dataFile(std::string(nc_file_path), netCDF::NcFile::replace);
        code = to_netcdf(dataFile);

      } catch (netCDF::exceptions::NcException &e) {
        std::cerr << e.what() << std::endl;
        code = nc_err;
      }

      return code;
    }

   protected:
    Attributes m_attributes;
    POLAR_TYPE m_polar_type;

    PolarMap m_polars_map;

    static inline std::mutex m_mutex;
  };

}  // poem

#endif //POEM_POLARSET_H
