//
// Created by frongere on 21/01/25.
//

#include "Dimension.h"

namespace poem {

  Dimension::Dimension(const std::string &name, const std::string &unit, const std::string &description) :
      Dimensional(unit),
      m_description(description),
      m_name(name) {}

  const std::string &Dimension::name() const {
    return m_name;
  }

  void Dimension::change_name(const std::string &name) {
    m_name = name;
  }

  const std::string &Dimension::description() const {
    return m_description;
  }

  void Dimension::change_description(const std::string &description) {
    m_description = description;
  }

}  // poem
