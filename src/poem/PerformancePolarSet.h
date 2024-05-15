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

   public:
    PerformancePolarSet(const Attributes &attributes) :
        m_has_ppp(false),
        m_has_hppp(false),
        m_has_hvpp_pb(false),
        m_has_hvpp_hp(false),
        m_has_vpp(false),
        m_attributes(attributes) {}

    void initialize_ppp() {
      m_ppp = std::make_shared<PolarSet>();
      m_has_ppp = true;
    }

    void initialize_hppp() {
      m_hppp = std::make_shared<PolarSet>();
      m_has_hppp = true;
    }

    void initialize_hvpp_pb() {
      m_hvpp_pb = std::make_shared<PolarSet>();
      m_has_hvpp_pb = true;
    }

    void initialize_hvpp_hp() {
      m_hvpp_hp = std::make_shared<PolarSet>();
      m_has_hvpp_hp = true;
    }

    void initialize_vpp() {
      m_vpp = std::make_shared<PolarSet>();
      m_has_vpp = true;
    }

    bool has_ppp() const { return m_has_ppp; }

    bool has_hppp() const { return m_has_hppp; }

    bool has_hvpp_pb() const { return m_has_hvpp_pb; }

    bool has_hvpp_hp() const { return m_has_hvpp_hp; }

    bool has_vpp() const { return m_has_vpp; }

    std::shared_ptr<PolarSet> ppp() const { return m_ppp; }

    std::shared_ptr<PolarSet> hppp() const { return m_hppp; }

    std::shared_ptr<PolarSet> hvpp_pb() const { return m_hvpp_pb; }

    std::shared_ptr<PolarSet> hvpp_hp() const { return m_hvpp_hp; }

    std::shared_ptr<PolarSet> vpp() const { return m_vpp; }

    int to_netcdf(const std::string &nc_filename, bool use_groups, bool verbose=false) {

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

        if (has_ppp()) {
          auto group = use_groups ? dataFile.addGroup("ppp") : dataFile;

          m_ppp->to_netcdf(group); // Remplacer par dataFile pour ne pas avoir de group
          if (use_groups) {
            group.putAtt("polar_type", "Power Prediction (no sails)");
          }
        }

        if (has_hppp()) {
          auto group = use_groups ? dataFile.addGroup("hppp") : dataFile;

          m_hppp->to_netcdf(group); // Remplacer par dataFile pour ne pas avoir de group
          if (use_groups) {
            group.putAtt("polar_type", "Hybrid Power Prediction (motor and sails)");
          }
        }

        if (has_hvpp_pb()) {
          auto group = use_groups ? dataFile.addGroup("hvpp-pb") : dataFile;
          m_hvpp_pb->to_netcdf(group); // Remplacer par dataFile pour ne pas avoir de group
          if (use_groups) {
            group.putAtt("polar_type", "Hybrid Velocity Prediction (motor and sails) -- Brake Power control");
          }
        }

        if (has_hvpp_hp()) {
          auto group = use_groups ? dataFile.addGroup("hvpp-hp") : dataFile;
          m_hvpp_hp->to_netcdf(group); // Remplacer par dataFile pour ne pas avoir de group
          if (use_groups) {
            group.putAtt("polar_type", "Hybrid Velocity Prediction (motor and sails) -- Handle Position control");
          }
        }

        if (has_vpp()) {
          auto group = use_groups ? dataFile.addGroup("vpp") : dataFile;
          m_vpp->to_netcdf(group); // Remplacer par dataFile pour ne pas avoir de group
          if (use_groups) {
            group.putAtt("polar_type", "Velocity Prediction (no motor)");
          }
        }

      } catch (netCDF::exceptions::NcException &e) {
        std::cerr << e.what() << std::endl;
        return nc_err;
      }

      return 0;
    }


   private:

    bool m_has_ppp;
    std::shared_ptr<PolarSet> m_ppp;

    bool m_has_hppp;
    std::shared_ptr<PolarSet> m_hppp;

    bool m_has_hvpp_pb;
    std::shared_ptr<PolarSet> m_hvpp_pb;

    bool m_has_hvpp_hp;
    std::shared_ptr<PolarSet> m_hvpp_hp;

    bool m_has_vpp;
    std::shared_ptr<PolarSet> m_vpp;

    Attributes m_attributes;

  };

}  // poem

#endif //POEM_PERFORMANCEPOLARSET_H
