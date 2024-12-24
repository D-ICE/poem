//
// Created by frongere on 20/12/24.
//

#ifndef POEM_POLARTREE_H
#define POEM_POLARTREE_H

#include <memory>
#include <filesystem>
#include <dtree/dtree.h>

#include "PolarTable.h"
#include "poem/exceptions.h"

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

  class PolarTree : public dtree::Node {
   public:
    PolarTree(const std::string& name) :
        dtree::Node(name),
        m_polar(nullptr) {}


   private:
    std::unique_ptr<Polar> m_polar;
  };


}  // poem

#endif //POEM_POLARTREE_H
