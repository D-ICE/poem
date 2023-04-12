//
// Created by frongere on 12/04/23.
//

#ifndef POEM_ATTRIBUTES_H
#define POEM_ATTRIBUTES_H

#include <map>

namespace poem {

  class Attributes {
   public:

    Attributes() = default;

    Attributes(const Attributes &other) = default;

    void add_attribute(const std::string &name, const std::string &val) {
      m_attributes.insert({name, val});
    }

    bool contains(const std::string &name) {
      return m_attributes.find(name) != m_attributes.end();
    }

    const std::string &get(const std::string &name) const { return m_attributes.at(name); }

   private:
    std::map<std::string, std::string> m_attributes;
  };


}  // poem

#endif //POEM_ATTRIBUTES_H
