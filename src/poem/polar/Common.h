//
// Created by frongere on 12/04/23.
//

#ifndef POEM_COMMON_H
#define POEM_COMMON_H

#include <string>

#include <dunits/dunits.h>

#include "poem/exceptions.h"

namespace poem {


  class Named {
   public:
    /**
     * Constructor
     * @param name the name of the dimension
     * @param unit the unit used for that dimension
     * @param description described the dimension
     * @param type the type of the enclosed data
     */
    Named(const std::string &name,
          const std::string &unit,
          const std::string &description) :
        m_name(name),
        m_unit(unit),
        m_description(description) {

      check_unit();
    }

    const std::string &name() const { return m_name; }

    const std::string &unit() const { return m_unit; }

    const std::string &description() const { return m_description; }


   private:
    void check_unit() {
      if (!dunits::UnitsChecker::getInstance().is_valid_unit(m_unit, true)) {
        spdlog::critical("Unit \"{}\" is not a valid unit as per dunits library.", m_unit);
        CRITICAL_ERROR
      }
    }

   protected:
    std::string m_name;
    std::string m_unit;
    std::string m_description;

  };

}

#endif //POEM_COMMON_H
