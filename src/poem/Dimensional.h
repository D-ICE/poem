//
// Created by frongere on 16/01/25.
//

#ifndef POEM_DIMENSIONAL_H
#define POEM_DIMENSIONAL_H

#include <string>
#include <dunits/dunits.h>

#include "exceptions.h"
#include "poem/version.h"

namespace poem {

  /**
   * Base class with name, unit and description
   */
  class Dimensional {
   public:

    /**
     * CTOR
     * @param unit the unit used for that dimension
     * @param description described the dimension
     */
    Dimensional(const std::string &unit);

    const std::string &unit() const;

    void change_unit(const std::string &new_unit);

    bool operator==(const Dimensional &other) const;

    bool operator!=(const Dimensional &other) const;

   private:
    void check_unit();

   protected:
    std::string m_unit;

  };

}  // poem

#endif //POEM_DIMENSIONAL_H
