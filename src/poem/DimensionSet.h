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

    explicit DimensionSet(const DimensionVector &dimensions);

    [[nodiscard]] size_t size() const;

    [[nodiscard]] const std::string &name(size_t i) const;

    [[nodiscard]] size_t index(const std::string &name) const;

    [[nodiscard]] bool contains(const std::string &name) const;

    [[nodiscard]] std::shared_ptr<Dimension> dimension(size_t i) const;

    [[nodiscard]] std::shared_ptr<Dimension> dimension(const std::string &name) const;

    bool operator==(const DimensionSet& other) const;

    bool operator!=(const DimensionSet& other) const;

    [[nodiscard]] DimensionSetConstIter begin() const;

    [[nodiscard]] DimensionSetConstIter end() const;

   private:
    DimensionVector m_dimensions;

  };

  inline std::shared_ptr<DimensionSet> make_dimension_set(const std::vector<std::shared_ptr<Dimension>> &dimensions) {
    return std::make_shared<DimensionSet>(dimensions);
  }

}  // poem

#endif //POEM_DIMENSIONSET_H
