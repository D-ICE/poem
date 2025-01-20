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

    std::shared_ptr<PolarSet> create_polar_set(const std::string& name) {}

    std::shared_ptr<PolarSet> polar_set() const {

      if (!is_leaf()) {
        spdlog::critical("This OperationMode {} cannot handle a PolarSet (not a leaf)", full_name().string());
        CRITICAL_ERROR_POEM
      }
      if (!m_polar_set) {
        spdlog::critical("In OperationMode {}, the PolarSet has not been set yet", full_name().string());
      }

      return m_polar_set;
    }

    void set_polar_set(std::shared_ptr<PolarSet> polar_set) {
      if (m_polar_set) {
        spdlog::critical("In OperationMode {}, the PolarSet already exists", full_name().string());
        CRITICAL_ERROR_POEM
      }
      m_polar_set = polar_set;
    }

    bool has_polar_set() const {
      return m_polar_set != nullptr;
    }

    bool operator==(const OperationMode &other) const {
      NIY_POEM
    }

    bool operator!=(const OperationMode &other) const {
      return !(other == *this);
    }

   private:
    std::shared_ptr<PolarSet> m_polar_set;
  };

}  // poem

#endif //POEM_OPERATIONMODE_H
