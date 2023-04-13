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

  template<size_t _dim>
  class DimensionPoint;

  template<size_t _dim>
  class DimensionIDArray {
   public:
    using Array = std::array<std::shared_ptr<DimensionID>, _dim>;

    explicit DimensionIDArray(const Array &array) : m_array(array) {}

    DimensionID *get(size_t i) const { return m_array.at(i).get(); }

    const size_t get_index(const std::string &name) const {
      for (size_t i = 0; i < _dim; ++i) {
        if (m_array.at(i)->name() == name) return i;
      }
      throw UnknownDimensionName();
    }

    std::shared_ptr<DimensionPoint<_dim>> get_point() const {
      return std::make_shared<DimensionPoint<_dim>>(this);
    }


   private:
    Array m_array;

  };

  template<size_t _dim>
  class DimensionPoint {
   public:
    using Values = std::array<double, _dim>;

    explicit DimensionPoint(const DimensionIDArray<_dim> *IDArray) :
        m_ID_array(IDArray),
        m_values({}) {}

    DimensionPoint(DimensionIDArray<_dim> *IDArray, const Values &values) :
        m_ID_array(IDArray),
        m_values(values) {

      Check();
    }

    void set(const Values &values) {
      m_values = values;
      Check();
    }

    const double &get(size_t i) const { return m_values.at(i); }

    const double &get(const std::string &name) {
      return m_values.at(m_ID_array->get_index(name));
    }

   private:
    void Check() {
      for (size_t i = 0; i < _dim; ++i) {
        const double val = m_values.at(i);
        DimensionID *ID = m_ID_array->get(i);
        if (val < ID->min() || val > ID->max()) {
          throw OutOfRangeDimension();
        }
      }
    }

   private:
    const DimensionIDArray<_dim> *m_ID_array;
    Values m_values;

  };


  /**
   * A sampled dimension
   */
  class DimensionSample {
   public:
    using Values = std::vector<double>;
    using Iter = Values::const_iterator;

    DimensionSample(DimensionID *ID, const Values &values) :
        m_ID(ID),
        m_values(values) {}

    size_t size() const { return m_values.size(); }

    Iter begin() const { return m_values.cbegin(); }

    Iter end() const { return m_values.cend(); }

    const double *data() const { return m_values.data(); }

   private:
    DimensionID *m_ID;
    std::vector<double> m_values;
  };

//  namespace internal {
//    template<size_t _dim, class Callable>
//    void meta_for_loop(std::array<std::vector<double>, _dim> vectors, Callable &&callable) {
//
//    }
//
//
//  }  // poem::internal

  template<size_t _dim>
  class DimensionSampleArray {
   public:
    explicit DimensionSampleArray(std::shared_ptr<DimensionIDArray<_dim>> dim_ID_array) :
        m_dim_ID_array(dim_ID_array) {}

    void set(const std::string &name, const std::vector<double> &values) {
      size_t i = m_dim_ID_array->get_index(name);
      m_vectors.at(i) = values;
    }

    std::vector<std::shared_ptr<DimensionPoint<_dim>>> get_dimension_points_vector() const {
      check_is_filled();

      std::vector<std::shared_ptr<DimensionPoint<_dim>>> points;

      typename DimensionPoint<_dim>::Values values;
      meta_for_loop<_dim>(points, values);

      return points;
    }

   private:
    // Adapted from https://stackoverflow.com/questions/34535795/n-dimensionally-nested-metaloops-with-templates
    template <size_t index>
    constexpr void meta_for_loop(std::vector<std::shared_ptr<DimensionPoint<_dim>>>& points,
                                 typename DimensionPoint<_dim>::Values &values) const {
      for (const auto& element : m_vectors[index - 1]) {
        values.at(index - 1) = element;
        if constexpr (index == 1) {
          auto point = std::make_shared<DimensionPoint<_dim>>(m_dim_ID_array.get(), values);
          points.push_back(point);

        } else {
          meta_for_loop<index - 1>(points, values);
        }
      }
    }

   private:
    void check_is_filled() const {
      for (const auto &vector: m_vectors) {
        if (vector.empty()) {
          throw std::runtime_error("DimensionSampleArray is not totally filled. Cannot generate DimensionPoint vector");
        }
      }
    }

   private:
    std::shared_ptr<DimensionIDArray<_dim>> m_dim_ID_array;
    std::array<std::vector<double>, _dim> m_vectors;
  };

}  // poem

#endif //POEM_DIMENSIONS_H
