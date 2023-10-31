//
// Created by frongere on 18/07/23.
//

#ifndef POEM_DIMENSIONS_H
#define POEM_DIMENSIONS_H

#include <vector>
#include <string>
#include <array>
#include <memory>
#include <unordered_map>

#include "Common.h"
#include "poem/exceptions.h"
#include "Splitter.h"

namespace poem {

  /**
   * Dimension declares the IDentity of a dimension
   */
  class Dimension : public Named {

   public:
    Dimension(const std::string &name,
              const std::string &unit,
              const std::string &description) :
        Named(name, unit, description, type::DOUBLE) {} // A dimension has always the type DOUBLE (interpolation coord)

  };

  template<size_t _dim>
  class DimensionSet {
   public:
    explicit DimensionSet(const std::array<std::shared_ptr<Dimension>, _dim> &dimension_array) :
        m_dimension_array(dimension_array) {

      for (size_t idim = 0; idim < _dim; ++idim) {
        m_map_stoi.insert({dimension_array.at(idim)->name(), idim});
      }

    }

    size_t index(const std::string &name) const {
      return m_map_stoi.at(name);
    }

    const std::string &name(size_t idim) const {
      return m_dimension_array.at(idim)->name();
    }

    const std::shared_ptr<Dimension> &at(const std::string &name) const {
      return m_dimension_array.at(m_map_stoi.at(name));
    }

    const std::shared_ptr<Dimension> &at(size_t idim) const {
      return m_dimension_array.at(idim);
    }

   private:
    std::array<std::shared_ptr<Dimension>, _dim> m_dimension_array;
    std::unordered_map<std::string, size_t> m_map_stoi;
  };

  template<size_t _dim>
  class DimensionPointSet;

  template<size_t _dim>
  class DimensionGrid {
   public:
    explicit DimensionGrid(std::shared_ptr<DimensionSet<_dim>> dimension_set) :
        m_dimension_set(dimension_set) {}

    void set_values(const std::string &name, const std::vector<double> &values) {
      size_t index;
      try {
        index = m_dimension_set->index(name);

      } catch (const std::out_of_range &e) {
        spdlog::critical("{} is not a valid dimension name", name);
        CRITICAL_ERROR_POEM
      }

      m_values.at(index) = values;

    }

    const std::vector<double> &values(size_t idx) const {
      return m_values.at(idx);
    }

    bool is_filled() const {
      for (const auto &values: m_values) {
        if (values.empty()) return false;
      }
      return true;
    }

    size_t size() const {
      size_t N = 1;
      for (size_t idim = 0; idim < _dim; ++idim) {
        if (m_values.empty()) {
          spdlog::critical("Value for dimension is not set");
          CRITICAL_ERROR_POEM
        }
        N *= m_values[idim].size();
      }
      return N;
    }

    std::shared_ptr<DimensionPointSet<_dim>> dimension_point_set() const {

      std::vector<std::array<double, _dim>> points_arrays;
      points_arrays.reserve(size());

      // Generating the dimension points
      std::array<double, _dim> array;
      nested_for_loop<_dim>(points_arrays, array);

      return std::make_shared<DimensionPointSet<_dim>>(m_dimension_set, points_arrays, *this);

    }

   private:
    // Adapted from https://stackoverflow.com/questions/34535795/n-dimensionally-nested-metaloops-with-templates
    // We use row major convention (last dimension varies the fastest) to be directly compliant with NetCDF internal
    // storage convention (and it is C compliant too...)
    template<size_t index>
    constexpr void nested_for_loop(std::vector<std::array<double, _dim>> &points_arrays,
                                   std::array<double, _dim> &array) const {
      size_t idim = _dim - index; // makes the last loop iterate on last dim
      for (const auto &value: m_values[idim]) {
        array.at(idim) = value;
        if constexpr (index == 1) {
          points_arrays.push_back(array);
        } else {
          nested_for_loop<index - 1>(points_arrays, array);
        }
      }
    }


   private:
    std::shared_ptr<DimensionSet<_dim>> m_dimension_set;
    std::array<std::vector<double>, _dim> m_values;

  };


  template<size_t _dim>
  class DimensionPoint {
   public:
    DimensionPoint(const DimensionSet<_dim> *dimension_set,
                   const std::array<double, _dim> *array) :
        m_dimension_set(dimension_set),
        m_array(array) {}

    const double &operator[](const std::string &name) const {
      return m_array->at(m_dimension_set->index(name));
    }

    const double &operator[](size_t idim) const {
      return m_array->at(idim);
    }

   private:
    const DimensionSet<_dim> *m_dimension_set;
    const std::array<double, _dim> *m_array;
  };


  class DimensionPointSetBase {
   public:
    virtual ~DimensionPointSetBase() = default; // To make the base class polymorphic (needed for the reader)
  };


  template<size_t _dim>
  class DimensionPointSet : public DimensionPointSetBase {
   public:
    explicit DimensionPointSet(std::shared_ptr<DimensionSet<_dim>> dimension_set,
                               const std::vector<std::array<double, _dim>> &points_array,
                               const DimensionGrid<_dim> &dimension_grid) :
        m_dimension_set(dimension_set),
        m_points_arrays(points_array),
        m_dimension_grid(dimension_grid) {

    }

    size_t size() const {
      return m_points_arrays.size();
    }

    DimensionPoint<_dim> dimension_point(size_t idx) const {
      return DimensionPoint<_dim>(m_dimension_set.get(), &m_points_arrays.at(idx));
    }

    const DimensionGrid<_dim> &dimension_grid() const {
      return m_dimension_grid;
    }

    std::shared_ptr<DimensionSet<_dim>> dimension_set() const {
      return m_dimension_set;
    }


   private:
    std::shared_ptr<DimensionSet<_dim>> m_dimension_set;
    std::vector<std::array<double, _dim>> m_points_arrays;

    DimensionGrid<_dim> m_dimension_grid;

  };


}  // poem

#endif //POEM_DIMENSIONS_H
