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
    PerformancePolarSet(const Attributes& attributes) :
        m_has_ppp(false),
        m_has_vpp(false),
        m_has_hvpp(false),
        m_attributes(attributes) {}

    void has_ppp(bool val) {
      m_has_ppp = val;
      if (val) {
        m_ppp = std::make_shared<PolarSet>();
      }
    }
    void has_vpp(bool val) {
      m_has_vpp = val;
      if (val) {
        m_vpp = std::make_shared<PolarSet>();
      }
    }
    void has_hvpp(bool val) {
      m_has_hvpp = val;
      if (val) {
        m_hvpp = std::make_shared<PolarSet>();
      }
    }

    bool has_ppp() const { return m_has_ppp; }
    bool has_vpp() const  { return m_has_vpp; }
    bool has_hvpp() const  { return m_has_hvpp; }

    std::shared_ptr<PolarSet> ppp() const { return m_ppp; }

    std::shared_ptr<PolarSet> vpp() const { return m_vpp; }

    std::shared_ptr<PolarSet> hvpp() const { return m_hvpp; }

    int to_netcdf(const std::string &nc_filename) {

      fs::path nc_file_path(nc_filename);
      if (nc_file_path.is_relative()) {
        nc_file_path = fs::current_path() / nc_file_path;
      }

      spdlog::info("Writing NetCDF file {}", std::string(nc_file_path));

      constexpr int nc_err = 2;

      try {

        // Create the file. The Replace parameter tells netCDF to overwrite
        // this file, if it already exists.
        netCDF::NcFile dataFile(std::string(nc_file_path), netCDF::NcFile::replace);

        for (const auto &attribute: m_attributes) {
          dataFile.putAtt(attribute.first, attribute.second);
        }

        bool use_groups = true; // Retirer a terme une fois que polarplot saura faire...

        if (has_ppp()) {
          auto group = use_groups ? dataFile.addGroup("ppp") : dataFile;

          m_ppp->to_netcdf(group); // Remplacer par dataFile pour ne pas avoir de group
          if (use_groups) {
            group.putAtt("polar_type", "Power Prediction");
          }
        }
        if (has_vpp()) {
          auto group = use_groups ? dataFile.addGroup("vpp") : dataFile;
          m_vpp->to_netcdf(group); // Remplacer par dataFile pour ne pas avoir de group
          if (use_groups) {
            group.putAtt("polar_type", "Velocity Prediction");
          }
        }
        if (has_hvpp()) {
          auto group = use_groups ? dataFile.addGroup("hvpp") : dataFile;
          m_hvpp->to_netcdf(group); // Remplacer par dataFile pour ne pas avoir de group
          if (use_groups) {
            group.putAtt("polar_type", "Hybrid Velocity Prediction");
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
    bool m_has_vpp;
    std::shared_ptr<PolarSet> m_vpp;
    bool m_has_hvpp;
    std::shared_ptr<PolarSet> m_hvpp;

    Attributes m_attributes;

  };

}  // poem

#endif //POEM_PERFORMANCEPOLARSET_H
