//
// Created by frongere on 31/10/23.
//

#ifndef POEM_PERFORMANCEPOLARSET_H
#define POEM_PERFORMANCEPOLARSET_H

#include <memory>
#include <filesystem>
#include <utility>

#include "polar.h"
#include "attributes.h"

namespace fs = std::filesystem;

namespace poem {
  /**
   * PolarSet is a set of Polar for a given vessel configuration.
   *
   * It can enclose the standard Polars (MPPP, HPPP, MVPP, HVPP, VPP)
   */
  class PolarSet {

    using PolarMap = std::unordered_map<POLAR_TYPE, std::shared_ptr<Polar> >;
    using PolarIter = PolarMap::iterator;
    using PolarConstIter = PolarMap::const_iterator;

   public:
    explicit PolarSet(const std::string &name,
                      Attributes attributes) :
        m_name(name),
        m_attributes(std::move(attributes)) {
    }

    const std::string &name() const {
      return m_name;
    }

    std::shared_ptr<Polar> new_polar(const Attributes &attributes, POLAR_TYPE polar_type) {

      if (m_polar_map.contains(polar_type)) {
        spdlog::critical("Polar of type {} already exists in PolarSet {}. "
                         "Attempting to create new one...",
                         polar_type, name());
        CRITICAL_ERROR_POEM
      }

      auto polar_set = std::make_shared<Polar>("", attributes, polar_type);
      m_polar_map.insert({polar_type, polar_set});
      return polar_set;
    }


    void add_polar(std::shared_ptr<Polar> polar_set) {
      auto polar_type = polar_set->polar_type();

      if (m_polar_map.contains(polar_type)) {
        spdlog::warn("Polar of type {} already exists in PolarSet {}. Assigning twice...",
                     polar_type, name());
      }

      m_polar_map.insert({polar_type, polar_set});

    }

    std::vector<POLAR_TYPE> polar_type_list() const {
      std::vector<POLAR_TYPE> list;
      list.reserve(m_polar_map.size());
      for (const auto &polar_set: m_polar_map) {
        list.push_back(polar_set.first);
      }
      return list;
    }

    std::vector<std::string> polar_list() const {
      std::vector<std::string> list;
      list.reserve(m_polar_map.size());
      for (const auto &polar_set: m_polar_map) {
        list.push_back(polar_type_enum2s(polar_set.first));
      }
      return list;
    }

    inline bool exist(POLAR_TYPE polar_type) const {
      return m_polar_map.contains(polar_type);
    }

    void rename_variables(const std::unordered_map<std::string, std::string> &varnames_map) {
      for (auto it = begin(); it != end(); it++) {
        it->second->rename_variables(varnames_map);
      }
    }

    std::shared_ptr<Polar> polar(POLAR_TYPE polar_type) const {
      std::shared_ptr<Polar> polar_set;
      if (exist(polar_type)) {
        polar_set = m_polar_map.at(polar_type);
      } else {
        spdlog::critical("Polar type {} does not exist in PolarSet {}. ",
                         polar_type_enum2s(polar_type), m_name);
      }
      return polar_set;
    }

    /*
     * Iterators
     */
    PolarIter begin() {
      return m_polar_map.begin();
    }

    PolarIter end() {
      return m_polar_map.end();
    }

    PolarConstIter begin() const {
      return m_polar_map.cbegin();
    }

    PolarConstIter end() const {
      return m_polar_map.cend();
    }

    int to_netcdf(const std::string &nc_filename, bool verbose = false) {
      fs::path nc_file_path(nc_filename);
      if (nc_file_path.is_relative()) {
        nc_file_path = fs::current_path() / nc_file_path;
      }

      if (verbose) spdlog::info("Writing NetCDF file {}", std::string(nc_file_path));

      constexpr int nc_err = 2;

      try {
        // Create the file. The replace parameter tells netCDF to overwrite
        // this file, if it already exists.
        netCDF::NcFile dataFile(std::string(nc_file_path), netCDF::NcFile::replace);

        for (const auto &attribute: m_attributes) {
          dataFile.putAtt(attribute.first, attribute.second);
        }

        for (const auto &polar_set: *this) {
          auto group = dataFile.addGroup(polar_set.second->polar_type_str());
          polar_set.second->to_netcdf(group);
        }

        dataFile.close();
      } catch (netCDF::exceptions::NcException &e) {
        std::cerr << e.what() << std::endl;
        return nc_err;
      }

      return 0;
    }

   private:
    std::string m_name;
    std::unordered_map<POLAR_TYPE, std::shared_ptr<Polar> > m_polar_map;

    Attributes m_attributes;
  };
} // poem

#endif // POEM_PERFORMANCEPOLARSET_H
