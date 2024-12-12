//
// Created by frongere on 31/10/23.
//

#ifndef POEM_PERFORMANCEPOLARSET_H
#define POEM_PERFORMANCEPOLARSET_H

#include <memory>
#include <filesystem>
#include <utility>

#include "polar_set.h"
#include "attributes.h"

namespace fs = std::filesystem;

namespace poem {
  class PerformancePolarSet {
    using PolarSetMap = std::unordered_map<POLAR_TYPE, std::shared_ptr<PolarSet> >;
    using PolarSetIter = PolarSetMap::iterator;
    using PolarSetConstIter = PolarSetMap::const_iterator;

    public:
      explicit PerformancePolarSet(const std::string &name,
                                   Attributes attributes) : m_name(name),
                                                            m_attributes(std::move(attributes)) {
      }

      const std::string &name() const {
        return m_name;
      }

      std::shared_ptr<PolarSet> new_polar_set(const Attributes &attributes, POLAR_TYPE polar_type) {
        if (m_polar_set_map.contains(polar_type)) {
          spdlog::warn("PolarSet of type {} already exists in PerformancePolarSet {}. "
                       "Attempting to create new one...",
                       polar_type, name());
        }

        NIY_POEM
        // if (exist(attributes["polar_type"])) {
        //   spdlog::critical("Attempting to create a PolarSet with name {} twice", attributes["name"]);
        //   CRITICAL_ERROR_POEM
        // }
        // auto polar_set = std::make_shared<PolarSet>("", attributes, polar_type);
        // m_polar_set_map.insert({attributes["polar_type"], polar_set});
        // return polar_set;
      }


      void AddPolarSet(std::shared_ptr<PolarSet> polar_set) {
        auto polar_type = polar_set->polar_type();

        if (m_polar_set_map.contains(polar_type)) {
          spdlog::warn("PolarSet of type {} already exists in PerformancePolarSet {}. Assigning twice...",
                       polar_type, name());
        }

        m_polar_set_map.insert({polar_type, polar_set});

        // NIY_POEM
        // // if (exist(polar_set->polar_type_str())) {
        // //   spdlog::critical("Attempting to add a PolarSet with name {} that is already present", polar_set->name());
        // //   CRITICAL_ERROR_POEM
        // // }
        // // m_polar_set_map.insert({polar_set->name(), polar_set});
      }

      std::vector<POLAR_TYPE> polar_set_type_list() const {
        std::vector<POLAR_TYPE> list;
        list.reserve(m_polar_set_map.size());
        for (const auto &polar_set: m_polar_set_map) {
          list.push_back(polar_set.first);
        }
        return list;
      }

      std::vector<std::string> polar_set_list() const {
        std::vector<std::string> list;
        list.reserve(m_polar_set_map.size());
        for (const auto &polar_set: m_polar_set_map) {
          list.push_back(polar_type_enum2s(polar_set.first));
        }
        return list;
      }

      // inline bool exist(const std::string &name) const {
      //   NIY_POEM
      //   // return m_polar_set_map.contains(name); // C++20
      // }

      inline bool exist(POLAR_TYPE polar_type) const {
        return m_polar_set_map.contains(polar_type);
      }

      void rename_variables(const std::unordered_map<std::string, std::string> varnames_map) {
        for (auto it = begin(); it != end(); it++) {
          it->second->rename_variables(varnames_map);
        }
      }

      std::shared_ptr<PolarSet> polar_set(const std::string &name) const {
        NIY_POEM
        // if (!exist(name)) {
        //   spdlog::critical("No PolarSet found with name {}", name);
        //   CRITICAL_ERROR_POEM
        // }
        // return m_polar_set_map.at(name);
      }

      PolarSetIter begin() {
        return m_polar_set_map.begin();
      }

      PolarSetIter end() {
        return m_polar_set_map.end();
      }

      PolarSetConstIter begin() const {
        return m_polar_set_map.cbegin();
      }

      PolarSetConstIter end() const {
        return m_polar_set_map.cend();
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
      std::unordered_map<POLAR_TYPE, std::shared_ptr<PolarSet> > m_polar_set_map;

      Attributes m_attributes;
  };
} // poem

#endif // POEM_PERFORMANCEPOLARSET_H
