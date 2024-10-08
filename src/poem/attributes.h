//
// Created by frongere on 18/04/23.
//

#ifndef POEM_ATTRIBUTES_H
#define POEM_ATTRIBUTES_H

#include <string>
#include <map>

namespace poem {

  /**
   * Represents different attributes (metadata) to be added to a PolarSet
   */
  class Attributes {
   public:
    using Map = std::map<std::string, std::string>;
    using Iter = Map::const_iterator;

    void add_attribute(const std::string &name, const std::string &val) {
      m_attributes.insert({name, val});
    }

    void add_attributes(const Attributes &other) {
      for (const auto &att: other) {
        m_attributes.insert(att);
      }
    }

    bool contains(const std::string &name) const {
      return m_attributes.find(name) != m_attributes.end();
    }

    const std::string &operator[](const std::string &name) const { return m_attributes.at(name); }

    const std::string &get(const std::string &name) const { return m_attributes.at(name); }

    void set(const std::string &name, const std::string &val) { m_attributes.at(name) = val; }

    Iter begin() const { return m_attributes.cbegin(); }

    Iter end() const { return m_attributes.cend(); }

   private:
    Map m_attributes;
  };

} // poem

#endif // POEM_ATTRIBUTES_H
