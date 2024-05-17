//
// Created by frongere on 31/10/23.
//

#ifndef POEM_PERFORMANCEPOLARSET_H
#define POEM_PERFORMANCEPOLARSET_H

#include <memory>
#include <filesystem>

#include "PolarSet.h"
#include "Attributes.h"

namespace fs = std::filesystem;

namespace poem {

  class PerformancePolarSet {
    using PolarSetMap = std::unordered_map<std::string, std::shared_ptr<PolarSet>>;
    using PolarSetIter = PolarSetMap::iterator;
    using PolarSetConstIter = PolarSetMap::const_iterator;

   public:
    explicit PerformancePolarSet(const Attributes &attributes) :
        m_attributes(attributes) {}

    std::shared_ptr<PolarSet> new_polar_set(const Attributes &attributes) {
      if (exist(attributes["name"])) {
        spdlog::critical("Attempting to create a PolarSet with name {} twice", attributes["name"]);
        CRITICAL_ERROR_POEM
      }
      auto polar_set = std::make_shared<PolarSet>(attributes);
      m_polar_set_map.insert({attributes["name"], polar_set});
      return polar_set;
    }

    const std::string& name() const {
      return m_attributes["name"];
    }

    void AddPolarSet(std::shared_ptr<PolarSet> polar_set) {
      if (exist(polar_set->name())) {
        spdlog::critical("Attempting to add a PolarSet with name {} that is already present", polar_set->name());
        CRITICAL_ERROR_POEM
      }
      m_polar_set_map.insert({polar_set->name(), polar_set});
    }

    std::vector<std::string> polar_set_list() const {
      std::vector<std::string> list;
      list.reserve(m_polar_set_map.size());
      for (const auto &polar_set: m_polar_set_map) {
        list.push_back(polar_set.first);
      }
      return list;
    }

    inline bool exist(const std::string &name) const {
      return m_polar_set_map.contains(name); // C++20
    }

    std::shared_ptr<PolarSet> polar_set(const std::string &name) const {
      if (!exist(name)) {
        spdlog::critical("Not PolarSet found with name {}", name);
        CRITICAL_ERROR_POEM
      }
      return m_polar_set_map.at(name);
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
          auto group = dataFile.addGroup(polar_set.first);
          polar_set.second->to_netcdf(group);
        }

        NIY_POEM


//        if (has_ppp()) {
//          auto group = use_groups ? dataFile.addGroup("ppp") : dataFile;
//
//          m_ppp->to_netcdf(group); // Remplacer par dataFile pour ne pas avoir de group
//          if (use_groups) {
//            group.putAtt("polar_type", "Power Prediction (no sails)");
//          }
//        }
//
//        if (has_hppp()) {
//          auto group = use_groups ? dataFile.addGroup("hppp") : dataFile;
//
//          m_hppp->to_netcdf(group); // Remplacer par dataFile pour ne pas avoir de group
//          if (use_groups) {
//            group.putAtt("polar_type", "Hybrid Power Prediction (motor and sails)");
//          }
//        }
//
//        if (has_hvpp_pb()) {
//          auto group = use_groups ? dataFile.addGroup("hvpp-pb") : dataFile;
//          m_hvpp_pb->to_netcdf(group);
//          if (use_groups) {
//            group.putAtt("polar_type", "Hybrid Velocity Prediction (motor and sails) -- Brake Power control");
//          }
//        }
//
//        if (has_hvpp_hp()) {
//          auto group = use_groups ? dataFile.addGroup("hvpp-hp") : dataFile;
//          m_hvpp_hp->to_netcdf(group);
//          if (use_groups) {
//            group.putAtt("polar_type", "Hybrid Velocity Prediction (motor and sails) -- Handle Position control");
//          }
//        }
//
//        if (has_vpp()) {
//          auto group = use_groups ? dataFile.addGroup("vpp") : dataFile;
//          m_vpp->to_netcdf(group); // Remplacer par dataFile pour ne pas avoir de group
//          if (use_groups) {
//            group.putAtt("polar_type", "Velocity Prediction (no motor)");
//          }
//        }

      } catch (netCDF::exceptions::NcException &e) {
        std::cerr << e.what() << std::endl;
        return nc_err;
      }

      return 0;
    }


   private:
    std::unordered_map<std::string, std::shared_ptr<PolarSet>> m_polar_set_map;
    Attributes m_attributes;

  };

}  // poem

#endif //POEM_PERFORMANCEPOLARSET_H
