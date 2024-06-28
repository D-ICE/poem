//
// Created by frongere on 27/05/24.
//

#ifndef POEM_SPECRULES_H
#define POEM_SPECRULES_H

#include <vector>
#include <netcdf>
#include <semver/semver.hpp>
#include "poem/version.h"

#include "SpecRulesBase.h"
#include "rules/rules.h"

namespace poem {

  static int max_poem_file_version = 1;

  class SpecRules : public SpecRulesBase {
   public:
    explicit SpecRules(int version_major) : SpecRulesBase(version_major) {
      if (version_major > max_poem_file_version) {
        spdlog::critical("Could not invoque Specification Rules for version {} (max version is currently {})",
                         version_major, max_poem_file_version);
        CRITICAL_ERROR_POEM
      }
      switch (version_major) {
        case 0:
          m_spec_rules = std::make_unique<v0::SpecRules>();
          break;
        case 1:
          m_spec_rules = std::make_unique<v1::SpecRules>();
          break;

        /// Add a case for each new major version

        default:
          spdlog::critical("POEM File Format Version {} does not exist", version_major);
          CRITICAL_ERROR_POEM
      }
    }

    [[nodiscard]] std::vector<std::string> coordinate_variables() const override {
      return m_spec_rules->coordinate_variables();
    }

    [[nodiscard]] std::vector<std::string> mandatory_variables() const override {
      return m_spec_rules->mandatory_variables();
    }

    [[nodiscard]] std::vector<std::string> understood_variables() const override {
      return m_spec_rules->understood_variables();
    }

    [[nodiscard]] bool check(const std::string &nc_polar_file, bool verbose) const override {
      return m_spec_rules->check(nc_polar_file, verbose);
    }

   private:
    std::unique_ptr<SpecRulesBase> m_spec_rules;
  };


//  class SpecRulesChecker {
//   public:
//    explicit SpecRulesChecker(const std::string &nc_polar, bool verbose) : m_polar_file(nc_polar), m_rules(0) {
//
//      // Does the file exist
//      if (!fs::exists(nc_polar)) {
//        spdlog::critical("File not found: {}", nc_polar);
//        CRITICAL_ERROR_POEM
//      }
//
//      semver::version current_poem_version;
//      try {
//        current_poem_version = semver::version::parse(git::LastTag(), false);
//      } catch (const semver::semver_exception &e) {
//        spdlog::critical("Current POEM version {} is not a valid version (does not follow semantic versioning)",
//                         git::LastTag());
//        CRITICAL_ERROR_POEM
//      }
//      if (verbose) spdlog::info("Current POEM version is v{}", current_poem_version.str());
//
//      // Get the version of the file
//      netCDF::NcFile ncfile(nc_polar, netCDF::NcFile::read);
//      std::string file_poem_version_str;
//      if (ncfile.getAtts().contains("poem_file_format_version")) {
//        ncfile.getAtt("poem_file_format_version").getValues(file_poem_version_str);
//      } else {
//        // poem_file_format_version was not a mandatory attribute
//        file_poem_version_str = "v0";
//      }
//      ncfile.close();
//
//      m_poem_file_version = semver::version::parse(file_poem_version_str, false);
//
//      if (current_poem_version < m_poem_file_version) {
//        spdlog::critical("POEM version is v{} while trying to decode POEM File writen with POEM version v{}",
//                         current_poem_version.str(), m_poem_file_version.str());
//        spdlog::critical("Please upgrade your code with POEM version at least v{}",
//                         semver::version(m_poem_file_version.major()).str());
//        CRITICAL_ERROR_POEM
//      }
//
//      m_rules = SpecRules(m_poem_file_version.major());
//
//    }
//
//    const semver::version &version() const {
//      return m_poem_file_version;
//    }
//
//    bool check(bool verbose) const {
//      return m_rules.check_rules(m_polar_file, verbose);
//    }
//
//    [[nodiscard]] std::vector<std::string> coordinate_variables() const {
//      return m_rules.coordinate_variables();
//    }
//
//    [[nodiscard]] std::vector<std::string> mandatory_variables() const {
//      return m_rules.mandatory_variables();
//    }
//
//    [[nodiscard]] std::vector<std::string> understood_variables() const {
//      return m_rules.understood_variables();
//    }
//
//   private:
//    std::string m_polar_file;
//    semver::version m_poem_file_version;
//    SpecRules m_rules;
//
//  };

}  // poem

#endif //POEM_SPECRULES_H
