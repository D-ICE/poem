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
     * @param unit the unit used for that dimension
     * @param description described the dimension
     */
    Named(const std::string &unit,
          const std::string &description);

    const std::string &unit() const;

    void change_unit(const std::string &new_unit);

    const std::string &description() const;

    void change_description(const std::string &new_description);

    bool operator==(const Named &other) const;

    bool operator!=(const Named &other) const;

   private:
    void check_unit();

   protected:
    std::string m_unit;
    std::string m_description;

  };

}  // poem

#endif //POEM_NAMED_H
