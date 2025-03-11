//
// Created by frongere on 21/01/25.
//

#include "PolarNode.h"

#include <cools/string/StringUtils.h>

#include "Dimension.h"
#include "PolarSet.h"
#include "Polar.h"
#include "PolarTable.h"

namespace poem {

  PolarNode::PolarNode(const std::string &name, const std::string &description) :
      dtree::Node(name),
      m_description(description),
      m_polar_node_type(POLAR_NODE) {

    // Ensure name is correct
    std::string name_(name);
    cools::string::MakeItAValidVariableName(name_);

    if (name_ != name) {
      change_name(name_);
      LogWarningError(R"(Name "{}" was not compliant and has been replaced by "{}")", name, name_);
    }

    if (name_.empty()) {
      LogCriticalError("Empty name for PolarNode is forbidden");
      CRITICAL_ERROR_POEM
    }
  }

  void PolarNode::change_name(const std::string &new_name) {
    rename(new_name);
  }

  const std::string &PolarNode::description() const { return m_description; }

  void PolarNode::change_description(const std::string &new_description) { m_description = new_description; }

  POLAR_NODE_TYPE PolarNode::polar_node_type() const {
    return m_polar_node_type;
  }

  std::shared_ptr<PolarSet> PolarNode::as_polar_set() {
    if (m_polar_node_type != POLAR_SET) {
      LogCriticalError("PolarNode {} is not a PolarSet", m_name);
      CRITICAL_ERROR_POEM
    }
    return std::dynamic_pointer_cast<PolarSet>(shared_from_this());
  }

  std::shared_ptr<Polar> PolarNode::as_polar() {
    if (m_polar_node_type != POLAR) {
      LogCriticalError("PolarNode {} is not a Polar", m_name);
      CRITICAL_ERROR_POEM
    }
    return std::dynamic_pointer_cast<Polar>(shared_from_this());
  }

  std::shared_ptr<PolarTableBase> PolarNode::as_polar_table() {
    if (m_polar_node_type != POLAR_TABLE) {
      LogCriticalError("PolarNode {} is not a PolarTableBase", m_name);
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
    } else {
      for (const auto &child: children<PolarNode>()) {
        child->polar_tables_paths(paths);
      }
    }

//    if (!is_leaf()) {
//      for (const auto &child: children<PolarNode>()) {
//        child->polar_tables_paths(paths);
//      }
//    }

  }

  std::shared_ptr<PolarNode> PolarNode::polar_node_from_path(const fs::path &path) {

    fs::path path_ = path;
    if (path_.is_absolute()) {
      if (path == "/") {
        if (is_root()) {
          path_ = m_name;
        } else {
          LogCriticalError("To use / as a shortcut for /VESSEL_NAME, current node must be root");
          CRITICAL_ERROR_POEM
        }
      } else {
        path_ = path_.relative_path();
      }
    }

    auto iter = path_.begin();
    auto current_node_name = (*iter).string();
    if ((*iter).string() != m_name) {
      LogCriticalError("In PolarNode {} and path mismatch: {}", m_name, path_.string());
      CRITICAL_ERROR_POEM
    }

    std::shared_ptr<PolarNode> polar_node;

    iter++;
    if (iter == path_.end()) {
      polar_node = std::dynamic_pointer_cast<PolarNode>(shared_from_this());

    } else {
      auto next_node_name = (*iter).string();
      fs::path next_path;
      for (; iter != path_.end(); ++iter) {
        next_path = next_path / *iter;
      }
      polar_node = child<PolarNode>(next_node_name)->polar_node_from_path(next_path);
    }

    return polar_node;
  }

  bool PolarNode::exists(const fs::path &path) {
    bool exists;
    if (path == "/") {
      return true;
    }

    try {
      polar_node_from_path(path);
    } catch (const std::exception &e) {
      return false;
    }

    return true;
  }

  json PolarNode::layout() const {
    json node;

    std::vector<std::string> paths;
    polar_tables_paths(paths);

    for (const auto &path: paths) {

      auto polar_table = const_cast<PolarNode *>(this)->polar_node_from_path(path)->as_polar_table();

      // Register DimensionGrid from parent
      auto parent = polar_table->parent<Polar>();
      auto parent_full_name = parent->full_name();

      if (!node["dimension_grids"].contains(parent_full_name)) {
        // DimensionGrid of parent does not exist yet

        auto dimension_grid = parent->dimension_grid();

        std::vector<std::string> dimensions;
        dimensions.reserve(dimension_grid->dimension_set()->size());
        for (const auto &dimension: *dimension_grid->dimension_set()) {
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

  #ifdef POEM_JIT

  void PolarNode::jit_load() {
    for (const auto &child: children<PolarNode>()) {
      child->jit_load();
    }
  }

  void PolarNode::jit_unload() {
    for (const auto &child: children<PolarNode>()) {
      child->jit_unload();
    }
  }

  int PolarNode::memsize() const {
    int size = sizeof(*this);
    for (const auto &child : children<PolarNode>()) {
      size += child->memsize();
    }
    return size;
  }

  #endif //POEM_JIT


  std::shared_ptr<PolarNode> make_polar_node(const std::string &name, const std::string &description) {
    return std::make_shared<PolarNode>(name, description);
  }

  std::shared_ptr<PolarNode> mount(const std::shared_ptr<PolarNode> &from_node, const std::string &from_path,
                                   const std::shared_ptr<PolarNode> &to_node, const std::string &to_path) {
    NIY_POEM
  }

}  // poem
