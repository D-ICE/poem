//
// Created by frongere on 21/01/25.
//

#include "DimensionSet.h"
#include "Dimension.h"

namespace poem {


  DimensionSet::DimensionSet(const DimensionSet::DimensionVector &dimensions) : m_dimensions(dimensions) {}

  size_t DimensionSet::size() const { return m_dimensions.size(); }

  const std::string &DimensionSet::name(size_t i) const { return m_dimensions.at(i)->name(); }

  size_t DimensionSet::index(const std::string &name) const {
    auto it = std::find_if(m_dimensions.begin(), m_dimensions.end(),
                           [name](const std::shared_ptr<Dimension>& dim) -> bool {
                             return dim->name() == name;
                           });

    if (it == m_dimensions.end()) {
      LogCriticalError("In DimensionSet, dimension {} not found", name);
      CRITICAL_ERROR_POEM
    }
    return it - m_dimensions.begin(); // TODO: tester
  }

  bool DimensionSet::contains(const std::string &name) const {
    auto it = std::find_if(m_dimensions.begin(), m_dimensions.end(),
                           [name](const std::shared_ptr<Dimension>& dim) -> bool {
                             return dim->name() == name;
                           });
    return it != m_dimensions.end(); // TODO: tester
  }

  std::shared_ptr<Dimension> DimensionSet::dimension(size_t i) const {
    return m_dimensions.at(i);
  }

  std::shared_ptr<Dimension> DimensionSet::dimension(const std::string &name) const {
    return dimension(index(name));
  }

  bool DimensionSet::operator==(const DimensionSet &other) const {
    bool equal = size() == other.size();
    for (size_t i = 0; i < size(); ++i) {
      equal &= *m_dimensions[i] == *other.m_dimensions[i];
    }
    return equal;
  }

  bool DimensionSet::operator!=(const DimensionSet &other) const {
    return !(other == *this);
  }

  DimensionSet::DimensionSetConstIter DimensionSet::begin() const { return m_dimensions.begin(); }

  DimensionSet::DimensionSetConstIter DimensionSet::end() const { return m_dimensions.end(); }

}  // poem
