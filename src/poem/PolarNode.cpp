//
// Created by frongere on 21/01/25.
//

#include "PolarNode.h"

#include "PolarSet.h"
#include "Polar.h"
#include "PolarTable.h"

namespace poem {

  std::string clean_name(const std::string &str) {
    // FIXME: semblerait que la fonctionnalite soit aussi dans cools...
    // FIXME: removes also numeric characters

    std::string s = str;
    for (size_t i = 0; i < s.size(); i++) {
      if (s[i] == ' ') {
        s[i] = '_';
        continue;
      }
      if (s[i] == '_') continue;

      if (s[i] < 'A' || s[i] > 'Z' && s[i] < 'a' || s[i] > 'z') {
        s.erase(i, 1);
        i--;
      }
    }
    if (str != s) {
      spdlog::warn("Name \"{}\" has been changed in \"{}\"", str, s);
    }

    return s;
  }

  PolarNode::PolarNode(const std::string &name) :
      dtree::Node(name),
      m_polar_node_type(POLAR_NODE) {}

  void PolarNode::change_name(const std::string &new_name) {
    rename(new_name);
  }

  POLAR_NODE_TYPE PolarNode::polar_node_type() const {
    return m_polar_node_type;
  }

  std::shared_ptr<PolarSet> PolarNode::as_polar_set() {
    if (m_polar_node_type != POLAR_SET) {
      spdlog::critical("PolarNode {} is not a PolarSet", m_name);
      CRITICAL_ERROR_POEM
    }
    return std::dynamic_pointer_cast<PolarSet>(shared_from_this());
  }

  std::shared_ptr<Polar> PolarNode::as_polar() {
    if (m_polar_node_type != POLAR) {
      spdlog::critical("PolarNode {} is not a Polar", m_name);
      CRITICAL_ERROR_POEM
    }
    return std::dynamic_pointer_cast<Polar>(shared_from_this());
  }

  std::shared_ptr<PolarTableBase> PolarNode::as_polar_table() {
    if (m_polar_node_type != POLAR_TABLE) {
      spdlog::critical("PolarNode {} is not a PolarTableBase", m_name);
      CRITICAL_ERROR_POEM
    }
    return std::dynamic_pointer_cast<PolarTableBase>(shared_from_this());
  }

  bool PolarNode::operator==(const PolarNode &other) const {
    NIY_POEM
  }

  bool PolarNode::operator!=(const PolarNode &other) const {
    return !(other == *this);
  }

}  // poem
