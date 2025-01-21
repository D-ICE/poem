//
// Created by frongere on 16/01/25.
//

#ifndef POEM_NAMED_H
#define POEM_NAMED_H

#include <string>
#include <dunits/dunits.h>

#include "exceptions.h"
#include "poem/version.h"

namespace poem {

  /**
   * Base class with name, unit and description
   */
  class Named {
   public:

    /**
     * CTOR
     * @param name the name of the dimension
     * @param unit the unit used for that dimension
     * @param description described the dimension
     */
    Named(const std::string &name,
          const std::string &unit,
          const std::string &description) :
        m_name(name),
        m_unit(unit),
        m_description(description) {

      check_unit();

      #ifndef ALLOW_DIRTY
      if (git::AnyUncommittedChanges()) {
        spdlog::critical("Using POEM with uncommitted code modifications is forbidden. Current POEM version: {}",
                         git::GetNormalizedVersionString());
        CRITICAL_ERROR_POEM
      }
      #endif
    }

    virtual const std::string &name() const { return m_name; }

    void change_name(const std::string &new_name) { m_name = new_name; }

    const std::string &unit() const { return m_unit; }

    void change_unit(const std::string &new_unit) { m_unit = new_unit; }

    const std::string &description() const { return m_description; }

    void change_description(const std::string &new_description) { m_description = new_description; }

    bool operator==(const Named &other) const {
      return m_name == other.m_name
             && m_unit == other.m_unit
             && m_description == other.m_description;
    }

    bool operator!=(const Named &other) const {
      return !(other == *this);
    }

   private:
    void check_unit() {
      if (!dunits::UnitsChecker::getInstance().is_valid_unit(m_unit, true)) {
        spdlog::critical("Unit \"{}\" is not a valid unit as per dunits library.", m_unit);
        CRITICAL_ERROR_POEM
      }
    }

   protected:
    std::string m_name;
    std::string m_unit;
    std::string m_description;

  };

}  // poem

#endif //POEM_NAMED_H
