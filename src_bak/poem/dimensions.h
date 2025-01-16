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

#include "common.h"
#include "poem/exceptions.h"
#include "splitter.h"

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
    using DimensionArray = std::array<std::shared_ptr<Dimension>, _dim>;
    using DimensionSetIter = DimensionArray::iterator;
    using DimensionSetConstIter = DimensionArray::const_iterator;

    explicit DimensionSet(const DimensionArray &dimension_array) :
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

    bool is_dim(const std::string &name) const {
      return m_map_stoi.find(name) != m_map_stoi.end(); // TODO: check...
    }

    const std::shared_ptr<Dimension> &at(const std::string &name) const {
      return m_dimension_array.at(m_map_stoi.at(name));
    }

    const std::shared_ptr<Dimension> &at(size_t idim) const {
      return m_dimension_array.at(idim);
    }

    std::vector<std::string> names() const {
      std::vector<std::string> list;
      for (const auto dim: m_dimension_array) {
        list.push_back(dim->name());
      }
      return list;
    }

    /*
     * Iterators
     */
    DimensionSetIter begin() {
      return m_dimension_array.begin();
    }

    DimensionSetIter end() {
      return m_dimension_array.end();
    }

    DimensionSetConstIter begin() const {
      return m_dimension_array.cbegin();
    }

    DimensionSetConstIter end() const {
      return m_dimension_array.cend();
    }

   private:
    DimensionArray m_dimension_array;
    std::unordered_map<std::string, size_t> m_map_stoi;
  };

//  template<size_t _dim>
//  class DimensionPoint;
//
//  template<size_t _dim>
//  inline std::ostream &operator<<(std::ostream &os, const DimensionPoint<_dim> &dimension_point) {
//    return dimension_point.cout(os);
//  }

  template<size_t _dim>
  class DimensionPoint {

   public:
    using DimensionPointArray = std::array<double, _dim>;
    using DimensionPointArrayIter = DimensionPointArray::iterator;
    using DimensionPointArrayConstIter = DimensionPointArray::const_iterator;


    DimensionPoint(const DimensionSet<_dim> *dimension_set,
                   const DimensionPointArray &array) :
        m_dimension_set(dimension_set),
        m_array(array) {}

    const double &operator[](const std::string &name) const {
      try {
        return m_array.at(m_dimension_set->index(name));
      } catch (const std::out_of_range &e) {
        spdlog::critical("[POEM] No dimension named {} in DimensionPoint", name);
        CRITICAL_ERROR_POEM
      }
    }

    const double &operator[](size_t idim) const {
      try {
        return m_array.at(idim);
      } catch (const std::out_of_range &e) {
        spdlog::critical("[POEM] index {} is out of range of DimensionPoint", idim);
        CRITICAL_ERROR_POEM
      }
    }

    const std::array<double, _dim> &array() const {
      return m_array;
    }

    /*
     * Iterators
     */

    DimensionPointArrayIter begin() {
      return m_array.begin();
    }

    DimensionPointArrayIter end() {
      return m_array.end();
    }

    DimensionPointArrayConstIter begin() const {
      return m_array.cbegin();
    }

    DimensionPointArrayConstIter end() const {
      return m_array.cend();
    }

    // FIXME: voir pourquoi on ne parvient pas a mettre cout private...
//   private:
//    friend std::ostream &operator<<(std::ostream &, const DimensionPoint<_dim> &);

    inline std::ostream &cout(std::ostream &os) const {
      for (size_t i = 0; i < _dim; ++i) {
        os << m_dimension_set->name(i) << ": " << m_array.at(i) << ";\t";
      }
      return os;
    }

   private:
    const DimensionSet<_dim> *m_dimension_set;
    const DimensionPointArray m_array;
  };


  template<size_t _dim>
  inline std::ostream &operator<<(std::ostream &os, const DimensionPoint<_dim> &dimension_point) {
    return dimension_point.cout(os);
  }


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

    const std::vector<double> &values(const std::string &name) const {
      size_t index;
      try {
        index = m_dimension_set->index(name);

      } catch (const std::out_of_range &e) {
        spdlog::critical("{} is not a valid dimension name", name);
        CRITICAL_ERROR_POEM
      }

      return m_values.at(index);
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

    std::shared_ptr<DimensionSet<_dim>> dimension_set() const {
      return m_dimension_set;
    }

    std::vector<DimensionPoint<_dim>> dimension_points() const {
      std::vector<DimensionPoint<_dim>> dimension_points;
      dimension_points.reserve(size());

      // Generating the dimension points
      std::array<double, _dim> array;
      nested_for_loop<_dim>(dimension_points, array);

      return dimension_points;
    }

   private:
    // Adapted from https://stackoverflow.com/questions/34535795/n-dimensionally-nested-metaloops-with-templates
    // We use row major convention (last dimension varies the fastest) to be directly compliant with NetCDF internal
    // storage convention (and it is C compliant too...)
    template<size_t index>
    constexpr void nested_for_loop(std::vector<DimensionPoint<_dim>> &dimension_points,
                                   std::array<double, _dim> &array) const {
      size_t idim = _dim - index; // makes the last loop iterate on last dim
      for (const auto &value: m_values[idim]) {
        array.at(idim) = value;
        if constexpr (index == 1) {
          dimension_points.push_back(DimensionPoint<_dim>(dimension_set().get(), array));
        } else {
          nested_for_loop<index - 1>(dimension_points, array);
        }
      }
    }


   private:
    std::shared_ptr<DimensionSet<_dim>> m_dimension_set;
    std::array<std::vector<double>, _dim> m_values;

  };


  class DimensionPointSetBase {
   public:
    virtual ~DimensionPointSetBase() = default; // To make the base class polymorphic (needed for the reader)
  };


  template<size_t _dim>
  class DimensionPointSet : public DimensionPointSetBase {

   public:
    using DimensionPointSetVector = std::vector<DimensionPoint<_dim>>;
    using DimensionPointSetIter = DimensionPointSetVector::iterator;
    using DimensionPointSetConstIter = DimensionPointSetVector::const_iterator;

    explicit DimensionPointSet(const DimensionGrid<_dim> &dimension_grid) :
        m_dimension_points(dimension_grid.dimension_points()),
        m_dimension_grid(dimension_grid) {}

    size_t size() const {
      return m_dimension_points.size();
    }

    DimensionPoint<_dim> dimension_point(size_t idx) const {
      return m_dimension_points.at(idx);
    }

    const DimensionGrid<_dim> &dimension_grid() const {
      return m_dimension_grid;
    }

    std::shared_ptr<DimensionSet<_dim>> dimension_set() const {
      return m_dimension_grid.dimension_set();
    }

    /*
     * Iterators
     */
    DimensionPointSetIter begin() {
      return m_dimension_points.begin();
    }

    DimensionPointSetIter end() {
      return m_dimension_points.end();
    }

    DimensionPointSetConstIter begin() const {
      return m_dimension_points.cbegin();
    }

    DimensionPointSetConstIter end() const {
      return m_dimension_points.cend();
    }


   private:
    DimensionPointSetVector m_dimension_points;
    DimensionGrid<_dim> m_dimension_grid;

  };


} // poem

#endif // POEM_DIMENSIONS_H
