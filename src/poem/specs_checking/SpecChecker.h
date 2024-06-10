//
// Created by frongere on 27/05/24.
//

#ifndef POEM_SPECCHECKER_H
#define POEM_SPECCHECKER_H

#include <vector>
#include <netcdf>
#include <semver/semver.hpp>
#include "poem/version.h"
#include "rules/rules.h"

/**
 * Ici,on met en place un systeme de validation de fichier netCDF, pas de structure de données POEM... C'est different
 * On veut verifier que le fichier netCDF donne est bien compliant avec le set de rules qui definissent un POEM File
 *
 */

namespace poem {

  class SpecChecker {
   public:
    explicit SpecChecker(const std::string &nc_polar) : m_polar_file(nc_polar) {

      // Does the file exist
      if (!fs::exists(nc_polar)) {
        spdlog::critical("File not found: {}", nc_polar);
        CRITICAL_ERROR_POEM
      }

      semver::version current_poem_version;
      try {
        current_poem_version = semver::version::parse(version::LastTag(), false);
      } catch (const semver::semver_exception &e) {
        spdlog::critical("Current POEM version {} is not a valid version (does not follow semantic versioning)",
                         version::LastTag());
        CRITICAL_ERROR_POEM
      }
      spdlog::info("Current POEM version is v{}", current_poem_version.str());

      // Get the version of the file
      netCDF::NcFile ncfile(nc_polar, netCDF::NcFile::read);
      std::string file_poem_version_str;
      if (ncfile.getAtts().contains("poem_file_format_version")) {
        ncfile.getAtt("poem_file_format_version").getValues(file_poem_version_str);
      } else {
        // poem_file_format_version was not a mandatory attribute
        file_poem_version_str = "v0";
      }
      ncfile.close();

      m_poem_file_version = semver::version::parse(file_poem_version_str, false);

      if (current_poem_version < m_poem_file_version) {
        spdlog::critical("POEM version is v{} while trying to decode POEM File writen with POEM version v{}",
                         current_poem_version.str(), m_poem_file_version.str());
        spdlog::critical("Please upgrade your code with POEM version at least v{}",
                         semver::version(m_poem_file_version.major()).str());
        CRITICAL_ERROR_POEM
      }

    }

    bool check() const {

      bool compliant = false;

      switch (m_poem_file_version.major()) {
        case 0:
          compliant = v0::check_rules(m_polar_file);
          break;
        default:
          spdlog::critical("No specification rules available for POEM File format version {}",
                           m_poem_file_version.str());
          CRITICAL_ERROR_POEM
      }

      return compliant;
    }

    [[nodiscard]] std::vector<std::string> mandatory_variables() const {
      switch (m_poem_file_version.major()) {
        case 0:
          return v0::mandatory_variables();
        default:
          spdlog::critical("No specification rules available for POEM File format version {}",
                           m_poem_file_version.str());
          CRITICAL_ERROR_POEM
      }
    }

    [[nodiscard]] std::vector<std::string> understood_variables() const {
      switch (m_poem_file_version.major()) {
        case 0:
          return v0::understood_variables();
        default:
          spdlog::critical("No specification rules available for POEM File format version {}",
                           m_poem_file_version.str());
          CRITICAL_ERROR_POEM
      }
    }

   private:
    std::string m_polar_file;
    semver::version m_poem_file_version;

  };


}  // poem

#endif //POEM_SPECCHECKER_H
