//
// Created by frongere on 20/12/24.
//

#ifndef POEM_POLARNODE_H
#define POEM_POLARNODE_H

#include <filesystem>

#include <dtree/dtree.h>
#include <nlohmann/json.hpp>

#include "exceptions.h"
#include "enums.h"
#include "Attributes.h"

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace poem {

  /// Replaces spaces by _
  /// Removes special characters
  std::string clean_name(const std::string &str);

  // Forward declarations
  class PolarSet;

  class Polar;

  class PolarTableBase;

  template<typename T>
  class PolarTable;


  /**
   * This is a mode of operation of a vessel.
   *
   * It is a node of a tree structure. Only leafs must contain valid PolarSet
   */
  class PolarNode : public dtree::Node {
   public:

    explicit PolarNode(const std::string &name);

    void change_name(const std::string &new_name);

    POLAR_NODE_TYPE polar_node_type() const;

    std::shared_ptr<PolarSet> as_polar_set();

    std::shared_ptr<Polar> as_polar();

    std::shared_ptr<PolarTableBase> as_polar_table();

    bool operator==(const PolarNode &other) const;

    bool operator!=(const PolarNode &other) const;

    Attributes &attributes();

    const Attributes &attributes() const;

    json layout() const;

    void polar_tables_paths(std::vector<std::string> &paths) const;

    std::shared_ptr<PolarNode> from_path(const fs::path& path);

   protected:
    POLAR_NODE_TYPE m_polar_node_type;
    Attributes m_attributes;

  };

  std::shared_ptr<PolarNode> make_polar_node(const std::string& name);

}  // poem

#endif //POEM_POLARNODE_H
