//
// Created by frongere on 11/04/23.
//

#ifndef POEM_DIMENSIONS_H
#define POEM_DIMENSIONS_H

#include <vector>
#include <string>
#include <array>

#include "Common.h"

#include "poem/exceptions.h"

namespace poem {

  /**
   *
   */
  class DimensionID : public Named {
   public:
    DimensionID(const std::string &name,
                const std::string &unit,
                const std::string &description,
                double min,
                double max) :
        Named(name, unit, description, type::DOUBLE),
        m_min(min),
        m_max(max) {

      // TODO: c'est ici qu'on veut faire le check par rapport au schema
    }

    const double &min() const { return m_min; }

    const double &max() const { return m_max; }

   private:
    double m_min;
    double m_max;
  };

  /**
   *
   * @tparam _dim
   */
  template<size_t _dim>
  class DimensionIDSet {
   public:
    using IDSet = std::array<std::shared_ptr<DimensionID>, _dim>;

    explicit DimensionIDSet(const IDSet &array) :
        m_ID_set(array) {
    }

    DimensionID *get(size_t i) const { return m_ID_set.at(i).get(); }

    const size_t get_index(const std::string &name) const {
      for (size_t i = 0; i < _dim; ++i) {
        if (m_ID_set.at(i)->name() == name) return i;
      }
      spdlog::critical("Unknown dimension name {}", name);
      CRITICAL_ERROR
    }

   private:
    IDSet m_ID_set;

  };

  // Forward declaration
  template<size_t>
  class DimensionPointSet;

  /**
   *
   * @tparam _dim
   */
  template<size_t _dim>
  class DimensionPoint {
   public:
    using Values = std::array<double, _dim>;

    DimensionPoint(const DimensionIDSet<_dim> *IDArray, DimensionPointSet<_dim> *dimension_point_set) :
        m_ID_array(IDArray),
        m_values({}),
        m_origin_dimension_point_set(dimension_point_set) {}

    DimensionPoint(DimensionIDSet<_dim> *IDArray, DimensionPointSet<_dim> *dimension_point_set, const Values &values) :
        m_ID_array(IDArray),
        m_values(values),
        m_origin_dimension_point_set(dimension_point_set) {

      Check();
    }

    void set(const Values &values) {
      m_values = values;
      Check();
    }

    const DimensionPointSet<_dim> *origin_dimension_point_set() const { return m_origin_dimension_point_set; }

    const double &get(size_t i) const { return m_values.at(i); }

    const double &get(const std::string &name) {
      return m_values.at(m_ID_array->get_index(name));
    }

   private:
    void Check() {
      // Checking if the given values are well between min and max for each dimension
      for (size_t i = 0; i < _dim; ++i) {
        const double val = m_values.at(i);
        DimensionID *ID = m_ID_array->get(i);
        if (val < ID->min() || val > ID->max()) {
          spdlog::critical("For dimension {}, values {} is out of range [{}, {}]",
                           ID->name(), val, ID->min(), ID->max());
          CRITICAL_ERROR
        }
      }
    }

   private:
    const DimensionIDSet<_dim> *m_ID_array;
    Values m_values;

    DimensionPointSet<_dim> *m_origin_dimension_point_set;

  };

  struct DimensionPointSetBase {
    virtual ~DimensionPointSetBase() = default;  // To make it polymorphic for usage of std::dynamic_cast in polar reader
  };

  /**
   *
   * @tparam _dim
   */
  template<size_t _dim>
  class DimensionPointSet : public DimensionPointSetBase {
   public:
    using DimensionPointVector = std::vector<std::shared_ptr<DimensionPoint<_dim>>>;
    using Iter = typename DimensionPointVector::const_iterator;

    explicit DimensionPointSet(std::shared_ptr<DimensionIDSet<_dim>> dim_ID_array) :
        m_dimension_ID_set(dim_ID_array) {}

    const DimensionIDSet<_dim> *dimension_ID_set() const { return m_dimension_ID_set.get(); }

    /**
     * Set vector of values for each dimension. Must be called _dim times.
     * @param name dimension name
     * @param values vector of values for the dimension (non-repeating increasing order)
     */
    void set_dimension_vector(const std::string &name, const std::vector<double> &values) {
      size_t i = m_dimension_ID_set->get_index(name);
      m_dimension_vectors.at(i) = values;
    }

    const std::vector<double> dimension_vector(size_t i) const { return m_dimension_vectors.at(i); }

    /**
     * Build the dimension point vector from vector of values for each dimension. It does the cartesian product of
     * dimensions by making the last dimension move faster (the last dimension is the most inner for loop).
     * Can only be called if every dimension has been filled with value vector. If it is not the case, a runtime exception
     * is thrown.
     * @return
     */
    void build() {
      check_is_filled();

      // Get the necessary size
      size_t size = 1;
      for (size_t i = 0; i < _dim; ++i) {
        size *= m_dimension_vectors.at(i).size();
      }
      m_dimension_points.reserve(size);

      typename DimensionPoint<_dim>::Values values;
      meta_for_loop<_dim>(values);

    }

    DimensionPoint<_dim> *at(size_t i) {
      return m_dimension_points.at(i).get();
    }

    Iter begin() const { return m_dimension_points.cbegin(); }

    Iter end() const { return m_dimension_points.cend(); }

    const size_t size() const { return m_dimension_points.size(); }


   private:
    // Adapted from https://stackoverflow.com/questions/34535795/n-dimensionally-nested-metaloops-with-templates
    // We use row major convention (last dimension varies the fastest) to be directly compliant with NetCDF internal
    // storage convention (and it is C compliant too...)
    template<size_t index>
    constexpr void meta_for_loop(typename DimensionPoint<_dim>::Values &values) {
      size_t i = _dim - index; // makes the last loop iterate on last dim
      for (const auto &element: m_dimension_vectors[i]) {
        values.at(i) = element;
        if constexpr (index == 1) {
          m_dimension_points.push_back(std::make_shared<DimensionPoint<_dim>>(m_dimension_ID_set.get(), this, values));

        } else {
          meta_for_loop<index - 1>(values);
        }
      }
    }

    void check_is_filled() const {
      for (const auto &vector: m_dimension_vectors) {
        if (vector.empty()) {
          throw std::runtime_error("DimensionSampleArray is not totally filled. Cannot generate DimensionPoint vector");
        }
      }
    }

   private:
    std::shared_ptr<DimensionIDSet<_dim>> m_dimension_ID_set;
    std::array<std::vector<double>, _dim> m_dimension_vectors;

    DimensionPointVector m_dimension_points;

  };

}  // poem

#endif //POEM_DIMENSIONS_H
