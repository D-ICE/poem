//
// Created by frongere on 20/12/24.
//

#ifndef POEM_OPERATIONMODE_H
#define POEM_OPERATIONMODE_H

#include <filesystem>

#include <dtree/dtree.h>

#include "exceptions.h"
#include "PolarSet.h"

namespace fs = std::filesystem;

namespace poem {

  /// Replaces spaces by _
  /// Removes special characters
  std::string clean_name(const std::string &str);

  /*
   * OperationMode repose sur dtree. Un noeud leaf contient un PolarSet et un DT
   *      PolarSet (contient des Polar, au plus 5)
   *          Polar (MPPP, HPPP, MVPP, HVPP, VPP)
   *              PolarTable
   */


  /**
   * This is a mode of operation of a vessel.
   *
   * It is a node of a tree structure. Only leafs must contain valid PolarSet
   */
  class OperationMode : public dtree::Node {
   public:

    explicit OperationMode(const std::string &name) :
        dtree::Node(name),
        m_polar_set(nullptr) {}

    std::shared_ptr<PolarSet> create_polar_set(const std::string& name);

    std::shared_ptr<PolarSet> polar_set() const;

    void set_polar_set(std::shared_ptr<PolarSet> polar_set);

    bool has_polar_set() const;

    bool operator==(const OperationMode &other) const;

    bool operator!=(const OperationMode &other) const;

   private:
    std::shared_ptr<PolarSet> m_polar_set;
  };

}  // poem

#endif //POEM_OPERATIONMODE_H
