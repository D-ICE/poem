//
// Created by frongere on 12/04/23.
//

#ifndef POEM_COMMON_H
#define POEM_COMMON_H

#include <string>
#include <utility>

#include <dunits/dunits.h>

#include "poem/exceptions.h"
#include "poem/version.h"

namespace poem {

  namespace type {
    enum POEM_TYPES {
      DOUBLE,
      INT
    };

  }  // poem::type

  class Named {
   public:
    /**
     * Constructor
     * @param name the name of the dimension
     * @param unit the unit used for that dimension
     * @param description described the dimension
     * @param type the type of the enclosed data
     */
    Named(std::string name,
          std::string unit,
          std::string description,
          type::POEM_TYPES type) :
        m_name(std::move(name)),
        m_unit(std::move(unit)),
        m_description(std::move(description)),
        m_type(type) {

      check_unit();

      #ifndef ALLOW_DIRTY
      if (git::AnyUncommittedChanges()) {
        spdlog::critical("Using POEM with uncommitted code modifications is forbidden. Current POEM version: {}",
                         git::GetNormalizedVersionString());
        CRITICAL_ERROR_POEM
      }
      #endif
    }

    const std::string &name() const { return m_name; }

    const std::string &unit() const { return m_unit; }

    const std::string &description() const { return m_description; }

    const type::POEM_TYPES &type() const { return m_type; }

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
    type::POEM_TYPES m_type;

  };

}

#endif //POEM_COMMON_H
