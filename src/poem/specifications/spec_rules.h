//
// Created by frongere on 27/05/24.
//

#ifndef POEM_SPECRULES_H
#define POEM_SPECRULES_H

#include <vector>
#include <netcdf>

#include "poem/version.h"
#include "spec_rules_base.h"
#include "rules/rules.h"

namespace poem {

  static int max_poem_file_version = 1;

  class SpecRules : public SpecRulesBase {
   public:
    explicit SpecRules(int version_major) : SpecRulesBase(version_major) {

      if (version_major > POEM_MAX_FILE_VERSION) {
        spdlog::critical("Could not invoque Specification Rules for version {} (max version is currently {})",
                         version_major, POEM_MAX_FILE_VERSION);
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

} // poem

#endif // POEM_SPECRULES_H
