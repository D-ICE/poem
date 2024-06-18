//
// Created by frongere on 27/05/24.
//

#ifndef POEM_SPECRULESCHECKER_H
#define POEM_SPECRULESCHECKER_H

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

  static int max_poem_file_version = 0;

  class SpecRules {
   public:
    explicit SpecRules(int version_major) : m_version_major(version_major) {
      if (version_major > max_poem_file_version) {
        spdlog::critical("Could not invoque Specification Rules for version {} (max version is currently {})",
                         version_major, max_poem_file_version);
        CRITICAL_ERROR_POEM
      }
    }

    bool check_rules(const std::string &nc_polar_file, bool verbose) const {
      bool compliant;
      switch (m_version_major) {
        case 0:
          compliant = poem::v0::check_rules(nc_polar_file, verbose);
          break;
          // Add here new cases when new specification version is released
      }
      return compliant;
    }

    [[nodiscard]] std::vector<std::string> coordinate_variables() const {
      std::vector<std::string> vars;
      switch (m_version_major) {
        case 0:
          vars = poem::v0::coordinate_variables();
          break;
          // Add here new cases when new specification version is released
      }
      return vars;
    }

    [[nodiscard]] std::vector<std::string> mandatory_variables() const {
      std::vector<std::string> vars;
      switch (m_version_major) {
        case 0:
          vars = poem::v0::mandatory_variables();
          break;
          // Add here new cases when new specification version is released
      }
      return vars;
    }

    [[nodiscard]] std::vector<std::string> understood_variables() const {
      std::vector<std::string> vars;
      switch (m_version_major) {
        case 0:
          vars = poem::v0::understood_variables();
          break;
          // Add here new cases when new specification version is released
      }
      return vars;
    }

   private:
    int m_version_major;
  };


  class SpecRulesChecker {
   public:
    explicit SpecRulesChecker(const std::string &nc_polar, bool verbose) : m_polar_file(nc_polar), m_rules(0) {

      // Does the file exist
      if (!fs::exists(nc_polar)) {
        spdlog::critical("File not found: {}", nc_polar);
        CRITICAL_ERROR_POEM
      }

      semver::version current_poem_version;
      try {
        current_poem_version = semver::version::parse(git::LastTag(), false);
      } catch (const semver::semver_exception &e) {
        spdlog::critical("Current POEM version {} is not a valid version (does not follow semantic versioning)",
                         git::LastTag());
        CRITICAL_ERROR_POEM
      }
      if (verbose) spdlog::info("Current POEM version is v{}", current_poem_version.str());

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

      m_rules = SpecRules(m_poem_file_version.major());

    }

    const semver::version &version() const {
      return m_poem_file_version;
    }

    bool check(bool verbose) const {
      return m_rules.check_rules(m_polar_file, verbose);
    }

    [[nodiscard]] std::vector<std::string> coordinate_variables() const {
      return m_rules.coordinate_variables();
    }

    [[nodiscard]] std::vector<std::string> mandatory_variables() const {
      return m_rules.mandatory_variables();
    }

    [[nodiscard]] std::vector<std::string> understood_variables() const {
      return m_rules.understood_variables();
    }

   private:
    std::string m_polar_file;
    semver::version m_poem_file_version;
    SpecRules m_rules;

  };


}  // poem

#endif //POEM_SPECRULESCHECKER_H
