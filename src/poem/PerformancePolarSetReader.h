//
// Created by frongere on 17/05/24.
//

#ifndef POEM_PERFORMANCEPOLARSETREADER_H
#define POEM_PERFORMANCEPOLARSETREADER_H

#include <string>
#include <filesystem>
#include <spdlog/spdlog.h>
#include <netcdf>

#include "exceptions.h"

namespace fs = std::filesystem;

namespace poem {

  class PerformancePolarSetReader {

   public:
    explicit PerformancePolarSetReader(const std::string &nc_polar) :
        m_ncfile(nc_polar, netCDF::NcFile::read) {

      // Get global attributes from the file
      Attributes attributes;
      for (const auto att : m_ncfile.getAtts()) {
        std::string att_val;
        att.second.getValues(att_val);
        attributes.add_attribute(att.first, att_val);
      }

      auto performance_polar_set = std::make_shared<PerformancePolarSet>(attributes);

      auto groups = m_ncfile.getGroups();
      for (const auto &group_ : groups) {
        std::string group_name = group_.first;

//        auto g = group_.second;
        Attributes attributes_;
        for (const auto& att : group_.second.getAtts()) {
          std::string att_val;
          att.second.getValues(att_val);
          attributes_.add_attribute(att.first, att_val);


        }

        NIY_POEM
      }
      // Get the groups


      NIY_POEM

    }

    bool has_groups() const {
      return false;
    }

    std::shared_ptr<PerformancePolarSet> performance_polar_set() const {
      NIY_POEM
    }

   private:
    netCDF::NcFile m_ncfile;
    std::shared_ptr<PerformancePolarSet> m_performance_polar_set;

  };

}  // poem

#endif //POEM_PERFORMANCEPOLARSETREADER_H
