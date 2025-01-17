//
// Created by frongere on 16/01/25.
//

#ifndef POEM_DIMENSIONGRID_H
#define POEM_DIMENSIONGRID_H

#include <memory>
#include <vector>

#include "exceptions.h"
#include "DimensionSet.h"
#include "DimensionPoint.h"

namespace poem {

  /**
   * Defines a numerical sampling for each of Dimension object in a DimensionSet
   */
  class DimensionGrid {
   public:
    explicit DimensionGrid(const std::shared_ptr<DimensionSet> &dimension_set) :
        m_dimension_set(dimension_set),
        m_dimensions_values(dimension_set->size()),
        m_is_initialized(false) {
    }

    void set_values(const std::string &name, const std::vector<double> &values) {

      if (!m_dimension_set->contains(name)) {
        spdlog::critical("Unknown dimension name {}", name);
        CRITICAL_ERROR_POEM
      }

      // Check that the values are in ascending order
      double prec = values.front() - 1.;
      for (const auto &val: values) {
        if (val <= prec) {
          spdlog::critical("Sampling values in DimensionGrid dimensions must be in stictly asscending order");
          CRITICAL_ERROR_POEM
        }
        prec = val;
      }

      m_dimensions_values.at(m_dimension_set->index(name)) = values;
      m_is_initialized = false;
    }

    const std::vector<double> &values(size_t idx) const {
      return m_dimensions_values.at(idx);
    }

    std::vector<double> &values(size_t idx) {
      return m_dimensions_values.at(idx);
    }

    const std::vector<double> &values(const std::string &name) const {
      return m_dimensions_values.at(m_dimension_set->index(name));
    }

    /**
     * Number of points in the grid
     * @return
     */
    size_t size() const {
      return dimension_points().size();
    }

    size_t size(size_t idx) const {
      return m_dimensions_values.at(idx).size();
    }

    size_t dim() const {
      return m_dimension_set->size();
    }

    std::vector<size_t> shape() const {
      std::vector<size_t> shape(dim());
      for (size_t idx = 0; idx < dim(); ++idx) {
        shape[idx] = size(idx);
      }
      return shape;
    }

    double min(size_t idim) const {
      return m_dimensions_values.at(idim).at(0);
    }

    double min(const std::string &name) const {
      return min(m_dimension_set->index(name));
    }

    double max(size_t idim) const {
      return m_dimensions_values.at(idim).at(size(idim) - 1);
    }

    double max(const std::string &name) const {
      return max(m_dimension_set->index(name));
    }

    bool operator==(const DimensionGrid &other) const {
      bool equal = *m_dimension_set == *(other.m_dimension_set);
      for (size_t i=0; i<dim(); ++i) {
        auto this_values = m_dimensions_values[i];
        auto other_values = other.m_dimensions_values[i];
        equal &= this_values == other_values;
      }
      equal &= m_dimension_points == other.m_dimension_points;
      return equal;
    }

    bool operator!=(const auto &other) const {
      return !(other == *this);
    }

    std::shared_ptr<DimensionSet> dimension_set() const { return m_dimension_set; }

    [[nodiscard]] const std::vector<DimensionPoint> &dimension_points() const {
      if (!m_is_initialized) {
        build_dimension_points();
      }
      return m_dimension_points;
    }

    [[nodiscard]] bool is_filled() const {
      struct IsEmpty {
        bool operator()(const std::vector<double> &values) {
          return !values.empty();
        }
      };
      return std::all_of(m_dimensions_values.begin(), m_dimensions_values.end(), IsEmpty());
    }

    std::shared_ptr<DimensionGrid> copy() const {
      auto new_dimension_grid = std::make_shared<DimensionGrid>(m_dimension_set);
      for (size_t idx = 0; idx < dim(); ++idx) {
        new_dimension_grid->set_values(m_dimension_set->dimension(idx)->name(), m_dimensions_values.at(idx));
      }
      return new_dimension_grid; // TODO: tester
    }

    size_t grid_to_index(const std::vector<size_t> &grid_indices) const {
      if (grid_indices.size() != dim()) {
        spdlog::critical("Number of indices is not equal to the number of dimensions of the DimensionGrid");
        CRITICAL_ERROR_POEM
      }

      size_t index = 0;
      size_t stride = 1;
      for (int i = dim() - 1; i >= 0; --i) {
        index += grid_indices[i] * stride;
        stride *= size(i);
      }

      return index;
    }

   private:
    void build_dimension_points() const {
      if (!is_filled()) {
        spdlog::critical("DimensionGrid is not fully filled");
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

    void nested_for_loop(DimensionPoint &dimension_point, size_t index) {
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

   private:
    std::shared_ptr<DimensionSet> m_dimension_set;
    std::vector<std::vector<double>> m_dimensions_values;
    bool m_is_initialized;
    std::vector<DimensionPoint> m_dimension_points;

  };

  std::shared_ptr<DimensionGrid> make_dimension_grid(const std::shared_ptr<DimensionSet> &dimension_set) {
    return std::make_shared<DimensionGrid>(dimension_set);
  }

}  // poem

#endif //POEM_DIMENSIONGRID_H
