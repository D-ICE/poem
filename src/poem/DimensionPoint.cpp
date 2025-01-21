//
// Created by frongere on 21/01/25.
//

#include "DimensionPoint.h"

namespace poem {

  std::shared_ptr<DimensionSet> DimensionPoint::dimension_set() const {
    return m_dimension_set;
  }

  size_t DimensionPoint::size() const {
    return m_dimension_set->size();
  }

  double &DimensionPoint::operator[](size_t i) { return m_values[i]; }

  const double &DimensionPoint::operator[](size_t i) const { return m_values[i]; }

  void DimensionPoint::operator=(const std::vector<double> &values) {
    if (values.size() != m_values.size()) {
      spdlog::critical("Attempt to fill a DimensionPoint with bad vector size ({} and {})",
                       values.size(), m_values.size());
      CRITICAL_ERROR_POEM
    }
    m_values = values;
  }

  bool DimensionPoint::operator==(const DimensionPoint &other) const {
    bool equal = *m_dimension_set == *other.m_dimension_set;
    equal &= m_values.size() == other.m_values.size();
    equal &= m_values == other.m_values;
    return equal;
  }

  bool DimensionPoint::operator!=(const DimensionPoint &other) const {
    return !(other == *this);
  }

  DimensionPoint::ValuesConstIter DimensionPoint::begin() const { return m_values.cbegin(); }

  DimensionPoint::ValuesConstIter DimensionPoint::end() const { return m_values.cend(); }

  std::ostream &DimensionPoint::cout(std::ostream &os) const {
    for (size_t i = 0; i < m_dimension_set->size(); ++i) {
      os << m_dimension_set->name(i) << ": " << m_values.at(i) << ";\t";
    }
    return os;
  }

  std::ostream &operator<<(std::ostream &os, const DimensionPoint &dimension_point) {
    return dimension_point.cout(os);
  }

}  // poem