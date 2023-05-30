//
// Created by frongere on 12/04/23.
//

#ifndef POEM_VARIABLES_H
#define POEM_VARIABLES_H


#include "Common.h"
#include "Dimensions.h"

namespace poem {

  class VariableID : public Named {
   public:
    VariableID(const std::string &name,
                const std::string &unit,
                const std::string &description) :
        Named(name, unit, description) {

      // TODO: c'est ici qu'on veut faire le check par rapport au schema
    }

  };

}  // poem

#endif //POEM_VARIABLES_H
