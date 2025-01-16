//
// Created by frongere on 16/01/25.
//

#ifndef POEM_DIMENSIONSET_H
#define POEM_DIMENSIONSET_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

#include "Dimension.h"

namespace poem {

  /**
   * Declares an ordered set of Dimension objects to be used as a basis to define the dimensions of a PolarTable
   */
  class DimensionSet {
   public:
    using DimensionVector = std::vector<std::shared_ptr<Dimension>>;
    using DimensionSetConstIter = DimensionVector::const_iterator;

    explicit DimensionSet(const DimensionVector dimensions) : m_dimensions(dimensions) {
      // Building map (acceleration)
      size_t idx = 0;
      for (const auto &dimension: dimensions) {
        m_map.insert({dimension->name(), idx});
        idx++;
      }
    }

    size_t size() const { return m_dimensions.size(); }

    const std::string &name(size_t i) const { return m_dimensions.at(i)->name(); }

    size_t index(const std::string &name) const {
      return m_map.at(name);
    }

    bool is_dim(const std::string &name) const {
      return m_map.contains(name);
    }

    std::shared_ptr<Dimension> dimension(size_t i) const {
      return m_dimensions.at(i);
    }

    std::shared_ptr<Dimension> dimension(const std::string &name) const {
      return dimension(index(name));
    }

    DimensionSetConstIter begin() const { return m_dimensions.begin(); }

    DimensionSetConstIter end() const { return m_dimensions.end(); }

   private:
    DimensionVector m_dimensions;
    std::unordered_map<std::string, size_t> m_map;

  };

  std::shared_ptr<DimensionSet> make_dimension_set(const std::vector<std::shared_ptr<Dimension>> dimensions) {
    return std::make_shared<DimensionSet>(dimensions);
  }

}  // poem

#endif //POEM_DIMENSIONSET_H
