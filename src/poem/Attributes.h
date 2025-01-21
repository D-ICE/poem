//
// Created by frongere on 21/01/25.
//

#ifndef POEM_ATTRIBUTES_H
#define POEM_ATTRIBUTES_H

#include <map>
#include <string>

namespace poem {

  /**
   * Represents different attributes (metadata) to be added to a Polar
   */
  class Attributes {
   public:
    using Map = std::map<std::string, std::string>;
    using Iter = Map::const_iterator;

    void add_attribute(const std::string &name, const std::string &val);

    void add_attributes(const Attributes &other);

    bool contains(const std::string &name) const;

    const std::string &operator[](const std::string &name) const;

    std::string &operator[](const std::string &name);

    const std::string &get(const std::string &name) const;

    void set(const std::string &name, const std::string &val);

    Iter begin() const;

    Iter end() const;

    bool operator==(const Attributes& other) const;
    bool operator!=(const Attributes& other) const;

   private:
    Map m_attributes;

  };

}  // poem

#endif //POEM_ATTRIBUTES_H
