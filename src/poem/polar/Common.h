//
// Created by frongere on 12/04/23.
//

#ifndef POEM_COMMON_H
#define POEM_COMMON_H

#include <string>

#include "poem/exceptions.h"

namespace poem {

  namespace type {
    enum POEM_TYPES {
      DOUBLE,
      INT
    };

  }  // poem::type

  class Named {
   public:
    Named(const std::string &name,
          const std::string &unit,
          const std::string &description,
          type::POEM_TYPES type) :
        m_name(name),
        m_unit(unit),
        m_description(description),
        m_type(type) {

      check_unit();
    }

    const std::string &name() const { return m_name; }

    const std::string &unit() const { return m_unit; }

    const std::string &description() const { return m_description; }

    const type::POEM_TYPES &type() const { return m_type; }

   private:
    void check_unit() {
      // TODO
    }

   protected:
    std::string m_name;
    std::string m_unit;
    std::string m_description;
    type::POEM_TYPES m_type;

  };

}

#endif //POEM_COMMON_H
