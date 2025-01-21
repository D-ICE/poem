//
// Created by frongere on 16/01/25.
//

#ifndef POEM_DIMENSIONSET_H
#define POEM_DIMENSIONSET_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace poem {

  // Forward declaration
  class Dimension;

  /**
   * Declares an ordered set of Dimension objects to be used as a basis to define the dimensions of a PolarTable
   */
  class DimensionSet {
   public:
    using DimensionVector = std::vector<std::shared_ptr<Dimension>>;
    using DimensionSetConstIter = DimensionVector::const_iterator;

    explicit DimensionSet(const DimensionVector dimensions);

    size_t size() const;

    const std::string &name(size_t i) const;

    size_t index(const std::string &name) const;

    bool contains(const std::string &name) const;

    std::shared_ptr<Dimension> dimension(size_t i) const;

    std::shared_ptr<Dimension> dimension(const std::string &name) const;

    bool operator==(const DimensionSet& other) const;

    bool operator!=(const DimensionSet& other) const;

    DimensionSetConstIter begin() const;

    DimensionSetConstIter end() const;

   private:
    DimensionVector m_dimensions;
    std::unordered_map<std::string, size_t> m_map;

  };

  inline std::shared_ptr<DimensionSet> make_dimension_set(const std::vector<std::shared_ptr<Dimension>> dimensions) {
    return std::make_shared<DimensionSet>(dimensions);
  }

}  // poem

#endif //POEM_DIMENSIONSET_H
