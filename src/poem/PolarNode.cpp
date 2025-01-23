//
// Created by frongere on 21/01/25.
//

#include "PolarNode.h"

#include "Dimension.h"
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
    bool equal = true;
    equal &= m_name == other.m_name;
    equal &= m_attributes == other.m_attributes;
    return equal;
  }

  bool PolarNode::operator!=(const PolarNode &other) const {
    return !(other == *this);
  }

  Attributes &PolarNode::attributes() {
    return m_attributes;
  }

  const Attributes &PolarNode::attributes() const {
    return m_attributes;
  }

  void PolarNode::polar_tables_paths(std::vector<std::string> &paths) const {

    if (m_polar_node_type == POLAR_TABLE) {
      paths.push_back(full_name());
    }

    if (!is_leaf()) {
      for (const auto &child: children<PolarNode>()) {
        child->polar_tables_paths(paths);
      }
    }
  }

  std::shared_ptr<PolarNode> PolarNode::from_path(const fs::path &path) {

    fs::path path_ = path;
    if (path_.is_absolute()) {
      path_ = path_.relative_path();
    }

    auto iter = path_.begin();
    auto current_node_name = (*iter).string();
    if ((*iter).string() != m_name) {
      spdlog::critical("In PolarNode {} and path mismatch: {}", m_name, path_.string());
      CRITICAL_ERROR_POEM
    }

    std::shared_ptr<PolarNode> polar_node;

    iter++;
    if (iter == path_.end()) {
      polar_node = std::dynamic_pointer_cast<PolarNode>(shared_from_this());

    } else {
      auto next_node_name = (*iter).string();
      fs::path next_path;
      for (; iter!=path_.end(); ++iter) {
        next_path = next_path / *iter;
      }
      polar_node = child<PolarNode>(next_node_name)->from_path(next_path);
    }

    return polar_node;
  }

  json PolarNode::layout() const {
    json node;

    std::vector<std::string> paths;
    polar_tables_paths(paths);

    for (const auto &path : paths) {

      auto polar_table = const_cast<PolarNode*>(this)->from_path(path)->as_polar_table();

      // Register DimensionGrid from parent
      auto parent = polar_table->parent<Polar>();
      auto parent_full_name = parent->full_name();

      if (!node["dimension_grids"].contains(parent_full_name)) {
        // DimensionGrid of parent does not exist yet

        auto dimension_grid = parent->dimension_grid();

        std::vector<std::string> dimensions;
        dimensions.reserve(dimension_grid->dimension_set()->size());
        for (const auto &dimension : *dimension_grid->dimension_set()) {
          dimensions.push_back(dimension->name());
          node["dimension_grids"][parent_full_name][dimension->name()]["unit"] = dimension->unit();
          node["dimension_grids"][parent_full_name][dimension->name()]["description"] = dimension->description();
          node["dimension_grids"][parent_full_name][dimension->name()]["values"] =
              dimension_grid->values(dimension->name());

        }
        node["dimension_grids"][parent_full_name]["dimensions"] = dimensions;
      }

      node["polar_tables"][path]["datatype"] = poem_datatype_to_string(polar_table->type());
      node["polar_tables"][path]["unit"] = polar_table->unit();
      node["polar_tables"][path]["description"] = polar_table->description();
      node["polar_tables"][path]["dimension_grid"] = parent_full_name;
      if (polar_table->attributes().contains("component")) {
        node["polar_tables"][path]["component"] = polar_table->attributes()["component"];
      } else {
        node["polar_tables"][path]["component"] = "None";
      }

    }

    return node;
  }

  std::shared_ptr<PolarNode> make_polar_node(const std::string &name) {
    return std::make_shared<PolarNode>(name);
  }

}  // poem
