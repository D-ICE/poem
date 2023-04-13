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
                const std::string &description,
                type::POEM_TYPES type) :
        Named(name, unit, description, type) {

      // TODO: c'est ici qu'on veut faire le check par rapport au schema
    }

  };

//  class VariableID : public Named {
//
//   public:
//    VariableID(const std::string &name,
//               const std::string &unit,
//               const std::string &description,
//               std::shared_ptr<Dimensions> dimensions) :
//        m_name(name),
//        m_dimensions(dimensions) {}
//
//    const std::string &name() const { return m_name; }
//
//    const Dimensions *dimensions() const { return m_dimensions.get(); }
//
//   private:
//    std::string m_name;
//    std::string m_unit;
//    std::string m_description;
//    std::shared_ptr<Dimensions> m_dimensions;
//
//  };
//
//  template<typename T>
//  class Variable : public VariableBase {
//   public:
//    Variable(const std::string &name, std::shared_ptr<Dimensions> dimensions) :
//        VariableBase(name, dimensions) {}
//
//   private:
//    std::vector<T> m_data;
//
//  };

}  // poem

#endif //POEM_VARIABLES_H
