//
// Created by frongere on 19/06/24.
//

#ifndef POEM_SPECRULESBASE_H
#define POEM_SPECRULESBASE_H

#include <string>
#include <vector>

namespace poem
{

  class SpecRulesBase
  {
  public:
    explicit SpecRulesBase(int version_major) : m_version_major(version_major) {}

    [[nodiscard]] int version() const { return m_version_major; }

    [[nodiscard]] virtual std::vector<std::string> coordinate_variables() const = 0;

    [[nodiscard]] virtual std::vector<std::string> mandatory_variables() const = 0;

    [[nodiscard]] virtual std::vector<std::string> understood_variables() const = 0;

    [[nodiscard]] virtual bool check(const std::string &nc_polar_file, bool verbose) const = 0;

  protected:
    int m_version_major;
  };

} // poem

#endif // POEM_SPECRULESBASE_H
