//
// Created by frongere on 20/12/24.
//

#ifndef POEM_POLARNODE_H
#define POEM_POLARNODE_H

#include <filesystem>

#include <dtree/dtree.h>

#include "exceptions.h"
#include "enums.h"

namespace fs = std::filesystem;

namespace poem {

  /// Replaces spaces by _
  /// Removes special characters
  std::string clean_name(const std::string &str);

  // Forward declarations
  class PolarSet;

  class Polar;

  class PolarTableBase;


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

   protected:
    POLAR_NODE_TYPE m_polar_node_type;

  };

}  // poem

#endif //POEM_POLARNODE_H
