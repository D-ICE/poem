//
// Created by frongere on 16/01/25.
//

#ifndef POEM_DIMENSIONPOINT_H
#define POEM_DIMENSIONPOINT_H

#include <ostream>
#include <vector>
#include <memory>

#include "exceptions.h"
#include "DimensionSet.h"

namespace poem {

  /**
  * A particular numerical realisation of a DimensionSet, ie a vector of values for each Dimension of a DimensionSet
  */
  class DimensionPoint {
    using Values = std::vector<double>;
    using ValuesConstIter = Values::const_iterator;

   public:
    explicit DimensionPoint(const std::shared_ptr<DimensionSet> &dimension_set) :
        m_dimension_set(dimension_set),
        m_values(dimension_set->size()) {}

    DimensionPoint(const std::shared_ptr<DimensionSet> &dimension_set, const std::vector<double> &values) :
        m_dimension_set(dimension_set),
        m_values(values) {}

    std::shared_ptr<DimensionSet> dimension_set() const;

    size_t size() const;

    double &operator[](size_t i);

    const double &operator[](size_t i) const;

    void operator=(const std::vector<double> &values);

    bool operator==(const DimensionPoint& other) const;

    bool operator!=(const DimensionPoint& other) const;

    /*
     * Iterators
     */
    ValuesConstIter begin() const;

    ValuesConstIter end() const;

    std::ostream &cout(std::ostream &os) const;

   private:
    std::shared_ptr<DimensionSet> m_dimension_set;
    Values m_values;

  };

  std::ostream &operator<<(std::ostream &os, const DimensionPoint &dimension_point);

}  // poem

#endif //POEM_DIMENSIONPOINT_H
