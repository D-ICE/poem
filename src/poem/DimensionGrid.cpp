//
// Created by frongere on 21/01/25.
//

#include "DimensionGrid.h"
#include "Dimension.h"
#include "DimensionSet.h"

namespace poem {

  void poem::DimensionGrid::set_values(const std::string &name, const std::vector<double> &values) {

    if (!m_dimension_set->contains(name)) {
      std::string available_dimensions;
      for (const auto &dim: *m_dimension_set) {
        available_dimensions += " " + dim->name();
      }
      LogCriticalError("In DimensionGrid, attempting to set values of an unknown dimension name {}. "
                       "Available Dimensions are ({})", name, available_dimensions);
      CRITICAL_ERROR_POEM
    }

    // Check that the values are in ascending order
    double prec = values.front() - 1.;
    for (const auto &val: values) {
      if (val <= prec) {
        LogCriticalError("Sampling values in DimensionGrid dimensions must be in stictly asscending order");
        CRITICAL_ERROR_POEM
      }
      prec = val;
    }

    m_dimensions_values.at(m_dimension_set->index(name)) = values;
    m_is_initialized = false;
  }

  const std::vector<double> &DimensionGrid::values(size_t idx) const {
    return m_dimensions_values.at(idx);
  }

  std::vector<double> &DimensionGrid::values(size_t idx) {
    return m_dimensions_values.at(idx);
  }

  const std::vector<double> &DimensionGrid::values(const std::string &name) const {
    return m_dimensions_values.at(m_dimension_set->index(name));
  }

  size_t DimensionGrid::size() const {
    return dimension_points().size();
  }

  size_t DimensionGrid::size(size_t idx) const {
    return m_dimensions_values.at(idx).size();
  }

  size_t DimensionGrid::size(const std::string &dim_name) const {
    return size(m_dimension_set->index(dim_name));
  }

  size_t DimensionGrid::ndims() const {
    return m_dimension_set->size();
  }

  std::vector<size_t> DimensionGrid::shape() const {
    std::vector<size_t> shape(ndims());
    for (size_t idx = 0; idx < ndims(); ++idx) {
      shape[idx] = size(idx);
    }
    return shape;
  }

  double DimensionGrid::min(size_t idim) const {
    return m_dimensions_values.at(idim).at(0);
  }

  double DimensionGrid::min(const std::string &name) const {
    return min(m_dimension_set->index(name));
  }

  double DimensionGrid::max(size_t idim) const {
    return m_dimensions_values.at(idim).at(size(idim) - 1);
  }

  double DimensionGrid::max(const std::string &name) const {
    return max(m_dimension_set->index(name));
  }

  bool DimensionGrid::operator==(const DimensionGrid &other) const {
    bool equal = *m_dimension_set == *(other.m_dimension_set);
    for (size_t i = 0; i < ndims(); ++i) {
      auto this_values = m_dimensions_values[i];
      auto other_values = other.m_dimensions_values[i];
      equal &= this_values == other_values;
    }
    equal &= m_dimension_points == other.m_dimension_points;
    return equal;
  }

  bool DimensionGrid::operator!=(const auto &other) const {
    return !(other == *this);
  }

  std::shared_ptr<DimensionSet> DimensionGrid::dimension_set() const { return m_dimension_set; }

  const std::vector<DimensionPoint> &DimensionGrid::dimension_points() const {
    if (!m_is_initialized) {
      build_dimension_points();
    }
    return m_dimension_points;
  }

  bool DimensionGrid::is_filled() const {
    struct IsEmpty {
      bool operator()(const std::vector<double> &values) {
        return !values.empty();
      }
    };
    return std::all_of(m_dimensions_values.begin(), m_dimensions_values.end(), IsEmpty());
  }

  std::shared_ptr<DimensionGrid> DimensionGrid::copy() const {
    auto new_dimension_grid = std::make_shared<DimensionGrid>(m_dimension_set);
    for (size_t idx = 0; idx < ndims(); ++idx) {
      new_dimension_grid->set_values(m_dimension_set->dimension(idx)->name(), m_dimensions_values.at(idx));
    }
    return new_dimension_grid; // TODO: tester
  }

  size_t DimensionGrid::grid_to_index(const std::vector<size_t> &grid_indices) const {
    if (grid_indices.size() != ndims()) {
      LogCriticalError("Number of indices is not equal to the number of dimensions of the DimensionGrid");
      CRITICAL_ERROR_POEM
    }

    for (size_t idx = 0; idx < ndims(); ++idx) {
      if (grid_indices[idx] < 0 || grid_indices[idx] >= size(idx)) {
        LogCriticalError("Coordinate index {} out of DimensionGrid range", idx);
        CRITICAL_ERROR_POEM
      }
    }

    size_t index = 0;
    size_t stride = 1;
    for (int i = ndims() - 1; i >= 0; --i) {
      index += grid_indices[i] * stride;
      stride *= size(i);
    }

    return index;
  }

  void DimensionGrid::build_dimension_points() const {
    if (!is_filled()) {
      LogCriticalError("DimensionGrid is not fully filled");
      CRITICAL_ERROR_POEM
    }

    auto self = const_cast<DimensionGrid *>(this);

    size_t size = 1.;
    for (const auto &values: m_dimensions_values) {
      size *= values.size();
    }
    self->m_dimension_points.reserve(size);

    DimensionPoint dimension_point(m_dimension_set);
    self->nested_for_loop(dimension_point, m_dimension_set->size());

    self->m_is_initialized = true;
  }

  void DimensionGrid::nested_for_loop(DimensionPoint &dimension_point, size_t index) {
    // We use row major convention (last dimension varies the fastest) to be directly compliant with NetCDF internal
    // storage convention (and it is C compliant too...)

    size_t idim = dimension_point.size() - index;
    for (const auto &value: m_dimensions_values[idim]) {
      dimension_point[idim] = value;
      if (index == 1) {
        m_dimension_points.push_back(dimension_point);
      } else {
        nested_for_loop(dimension_point, index - 1);
      }
    }
  }

}  // poem
