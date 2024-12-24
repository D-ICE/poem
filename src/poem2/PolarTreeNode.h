//
// Created by frongere on 20/12/24.
//

#ifndef POEM_POLARTREENODE_H
#define POEM_POLARTREENODE_H

#include <memory>
#include <filesystem>
#include <dtree/dtree.h>

#include "PolarTable.h"
#include "poem/exceptions.h"

/**
 * PolarTreeNode repose sur dtree. Un noeud leaf contient un PolarSet et un DT
 *      PolarSet (contient des Polar, au plus 5)
 *          Polar (MPPP, HPPP, MVPP, HVPP, VPP)
 *              PolarTable
 */

namespace fs = std::filesystem;

using namespace poem;

namespace poem2 {

  std::string clean_name(const std::string &str) {
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

  class PolarTreeNode : public dtree::Node {
   public:
    explicit PolarTreeNode(const std::string &name) :
        dtree::Node(name),
        m_polar_set(nullptr) {}

//    std::shared_ptr<Polar> create_polar(POLAR_TYPE type, std::shared_ptr<DimensionGrid> dimension_grid) {
//
//
//
//      if (!is_leaf()) {
//        spdlog::critical("Cannot create Polar on a non-leaf node ({})", name());
//        CRITICAL_ERROR_POEM
//      }
//      std::string polar_name = name() + ".PPP"; // TODO: convert type to string
//
//      m_polars.insert({type, std::make_shared<Polar>(polar_name, type, dimension_grid)});
//
//      return m_polars[type];
//    }


   private:
    std::shared_ptr<PolarSet> m_polar_set;
//    std::unordered_map<POLAR_TYPE, std::shared_ptr<Polar>> m_polars;
  };


}  // poem

#endif //POEM_POLARTREENODE_H
