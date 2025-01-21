//
// Created by frongere on 16/01/25.
//

#ifndef POEM_DIMENSION_H
#define POEM_DIMENSION_H

#include <string>
#include <memory>

#include "Named.h"

namespace poem {

  /**
   * Declares a polar table dimension with name, unit and description
   */
  class Dimension : public Named {
   public:
    Dimension(const std::string &name,
              const std::string &unit,
              const std::string &description) :
        Named(name, unit, description) {}
  };

  /**
   * Helper function to create a shared_ptr of Dimension
   * @param name
   * @param unit
   * @param description
   * @return
   */
  inline std::shared_ptr<Dimension> make_dimension(const std::string &name,
                                            const std::string &unit,
                                            const std::string &description) {
    return std::make_shared<Dimension>(name, unit, description);
  }

}  // poem

#endif //POEM_DIMENSION_H
