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

namespace poem {

  /**
   * Dimension declares the IDentity of a dimension
   */
  class Dimension : public Named {
   public:
    /**
     * Constructor
     * @param name the name of the dimension
     * @param unit the unit used for that dimension
     * @param description described the dimension
     * @param values the vector of values for the dimension
     */
    Dimension(const std::string &name,
              const std::string &unit,
              const std::string &description,
              const std::vector<double> &values) :
        Named(name, unit, description, type::DOUBLE), // A dimension has always the type DOUBLE (interpolation coord)
        m_values(values) {

    }

    size_t size() const {
      return m_values.size();
    }

    const std::vector<double> &values() const {
      return m_values;
    }

//    bool operator==(const Dimension &other) const {
//      if ((m_name != other.m_name) ||
//          (m_unit != other.m_unit) ||
//          (m_description != other.m_description)) {
//        return false;
//      }
//      return true;
//    }
//
//    bool operator!=(const Dimension &other) const {
//      return !(*this == other);
//    }

   private:
    std::vector<double> m_values;

  };

  template<size_t _dim>
  class DimensionArray {
   public:
    explicit DimensionArray(const std::array<std::shared_ptr<Dimension>, _dim> &array) :
        m_array(array) {
      for (size_t idim = 0; idim < _dim; ++idim) {
        m_map_stoi.insert({array.at(idim)->name(), idim});
        m_map_itos.insert({idim, array.at(idim)->name()});
      }
    }

    size_t index(const std::string &name) const {
      return m_map_stoi.at(name);
    }

    const std::string &name(size_t idim) const {
      return m_map_itos.at(idim);
    }

    const std::shared_ptr<Dimension> &at(const std::string &name) const {
      return m_array.at(m_map_stoi.at(name));
    }

    const std::shared_ptr<Dimension> &at(size_t idim) const {
      return m_array.at(idim);
    }

   private:
    std::array<std::shared_ptr<Dimension>, _dim> m_array;
    std::unordered_map<std::string, size_t> m_map_stoi;
    std::unordered_map<size_t, std::string> m_map_itos;
  };

//  // Forward declaration
//  template<size_t _dim>
//  class DimensionPointSet;

  /**
   *
   * @tparam _dim
   */
  template<size_t _dim>
  class DimensionPoint {
   public:
    DimensionPoint(const DimensionArray<_dim> *dimension_array,
                   const std::array<double, _dim> *array) :
        m_dimension_array(dimension_array),
        m_array(array) {}

    const double &operator[](const std::string &name) const {
      return m_array->at(m_dimension_array->index(name));
    }

    const double &operator[](size_t idim) const {
      return m_array->at(idim);
    }

   private:
    const DimensionArray<_dim> *m_dimension_array;
    const std::array<double, _dim> *m_array;
  };

  /**
   *
   * @tparam _dim
   */
  template<size_t _dim>
  class DimensionPointSet {
   public:
    explicit DimensionPointSet(const DimensionArray<_dim> &dimension_array) :
        m_dimension_array(dimension_array) {

      // Number of dimension points
      size_t size = 1;
      for (size_t idim = 0; idim < _dim; ++idim) {
        size *= m_dimension_array.at(idim)->size();
      }

      m_points_arrays.reserve(size);

      // Generating the dimension points
      std::array<double, _dim> array;
      nested_for_loop<_dim>(array);


    }

    DimensionPoint<_dim> dimension_point(size_t idx) const {
      return DimensionPoint<_dim>(&m_dimension_array, &m_points_arrays[idx]);
    }


   private:
    // Adapted from https://stackoverflow.com/questions/34535795/n-dimensionally-nested-metaloops-with-templates
    // We use row major convention (last dimension varies the fastest) to be directly compliant with NetCDF internal
    // storage convention (and it is C compliant too...)
    template<size_t index>
    constexpr void nested_for_loop(std::array<double, _dim> &array) {
      size_t idim = _dim - index; // makes the last loop iterate on last dim
      for (const auto &value: m_dimension_array.at(idim)->values()) {
        array.at(idim) = value;
        if constexpr (index == 1) {
          m_points_arrays.push_back(array);
        } else {
          nested_for_loop<index - 1>(array);
        }
      }
    }


   private:
//    friend class DimensionPoint<_dim>;

    DimensionArray<_dim> m_dimension_array;
    std::vector<std::array<double, _dim>> m_points_arrays;

  };


}  // poem

#endif //POEM_DIMENSIONS_H
