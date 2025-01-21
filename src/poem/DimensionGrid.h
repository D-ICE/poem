//
// Created by frongere on 16/01/25.
//

#ifndef POEM_DIMENSIONGRID_H
#define POEM_DIMENSIONGRID_H

#include <memory>
#include <vector>

#include "exceptions.h"
#include "DimensionPoint.h"

namespace poem {

  // Forward declaration
  class DimensionSet;

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

    void set_values(const std::string &name, const std::vector<double> &values);

    const std::vector<double> &values(size_t idx) const;

    std::vector<double> &values(size_t idx);

    const std::vector<double> &values(const std::string &name) const;

    /**
     * Number of points in the grid
     * @return
     */
    size_t size() const;

    size_t size(size_t idx) const;

    size_t size(const std::string& dim_name) const;

    size_t ndims() const;

    std::vector<size_t> shape() const;

    double min(size_t idim) const;

    double min(const std::string &name) const;

    double max(size_t idim) const;

    double max(const std::string &name) const;

    bool operator==(const DimensionGrid &other) const;

    bool operator!=(const auto &other) const;

    std::shared_ptr<DimensionSet> dimension_set() const;

    const std::vector<DimensionPoint> &dimension_points() const;

    bool is_filled() const;

    std::shared_ptr<DimensionGrid> copy() const;

    size_t grid_to_index(const std::vector<size_t> &grid_indices) const;

   private:
    void build_dimension_points() const;

    void nested_for_loop(DimensionPoint &dimension_point, size_t index);

   private:
    std::shared_ptr<DimensionSet> m_dimension_set;
    std::vector<std::vector<double>> m_dimensions_values;
    bool m_is_initialized;
    std::vector<DimensionPoint> m_dimension_points;

  };

  inline std::shared_ptr<DimensionGrid> make_dimension_grid(const std::shared_ptr<DimensionSet> &dimension_set) {
    return std::make_shared<DimensionGrid>(dimension_set);
  }

}  // poem

#endif //POEM_DIMENSIONGRID_H
