//
// Created by frongere on 21/01/25.
//

#include "Attributes.h"
#include "exceptions.h"

namespace poem {


  void Attributes::add_attribute(const std::string &name, const std::string &val) {
    m_attributes.insert({name, val});
  }

  void Attributes::add_attributes(const Attributes &other) {
    for (const auto &att: other) {
      m_attributes.insert(att);
    }
  }

  bool Attributes::remove_attribute(const std::string &name) {
    bool erased = false;
    if (m_attributes.contains(name)) {
      m_attributes.erase(name);
      erased = true;
    }
    return erased;
  }

  bool Attributes::contains(const std::string &name) const {
    return m_attributes.find(name) != m_attributes.end();
  }

  const std::string &Attributes::operator[](const std::string &name) const { return m_attributes.at(name); }

  std::string &Attributes::operator[](const std::string &name) { return m_attributes.at(name); }

  const std::string &Attributes::get(const std::string &name) const { return m_attributes.at(name); }

  void Attributes::set(const std::string &name, const std::string &val) { m_attributes.at(name) = val; }

  Attributes::Iter Attributes::begin() const { return m_attributes.cbegin(); }

  Attributes::Iter Attributes::end() const { return m_attributes.cend(); }

  bool Attributes::operator==(const Attributes &other) const {
    if (m_attributes.size() != other.m_attributes.size()) return false;
    for (const auto& attribute : m_attributes) {
      if (!other.m_attributes.contains(attribute.first)) return false;
      if (attribute.first == "date") continue;
      if (other.m_attributes.at(attribute.first) != attribute.second) return false;
    }
    return true;
  }

  bool Attributes::operator!=(const Attributes &other) const {
    return !(other == *this);
  }

}  // poem
