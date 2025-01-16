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
  * A particular point corresponding to a DimensionSet
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

    std::shared_ptr<DimensionSet> dimension_set() const {
      return m_dimension_set;
    }

    size_t size() const {
      return m_dimension_set->size();
    }

    double &operator[](size_t i) { return m_values[i]; }

    const double &operator[](size_t i) const { return m_values[i]; }

    void operator=(const std::vector<double> &values) {
      if (values.size() != m_values.size()) {
        spdlog::critical("Attempt to fill a DimensionPoint with bad vector size ({} and {})",
                         values.size(), m_values.size());
        CRITICAL_ERROR_POEM
      }
      m_values = values;
    }

    /*
     * Iterators
     */
    ValuesConstIter begin() const { return m_values.cbegin(); }

    ValuesConstIter end() const { return m_values.cend(); }


    std::ostream &cout(std::ostream &os) const {
      for (size_t i = 0; i < m_dimension_set->size(); ++i) {
        os << m_dimension_set->name(i) << ": " << m_values.at(i) << ";\t";
      }
      return os;
    }

   private:
    std::shared_ptr<DimensionSet> m_dimension_set;
    Values m_values;

  };

  std::ostream &operator<<(std::ostream &os, const DimensionPoint &dimension_point) {
    return dimension_point.cout(os);
  }

}  // poem

#endif //POEM_DIMENSIONPOINT_H
