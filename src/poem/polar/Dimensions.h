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
   * DimensionID declares the IDentity of a dimension
   */
  class DimensionID : public Named {
   public:
    /**
     * Constructor
     * @param name the name of the dimension
     * @param unit the unit used for that dimension
     * @param description described the dimension
     * @param min the minimum value of the dimension used in the polar table
     * @param max the maximum value of the dimension used in the polar table
     */
    DimensionID(const std::string &name,
                const std::string &unit,
                const std::string &description,
                double min,
                double max) :
        Named(name, unit, description, type::DOUBLE), // A dimension has always the type DOUBLE (interpolation coord)
        m_min(min),
        m_max(max) {

    }

    bool operator==(const DimensionID &other) const {
      if ((m_name != other.m_name) ||
          (m_unit != other.m_unit) ||
          (m_description != other.m_description) ||
          (m_min != other.m_min) ||
          (m_max != other.m_max)) {
        return false;
      }
      return true;
    }

    bool operator!=(const DimensionID &other) const {
      return !(*this == other);
    }

    const double &min() const { return m_min; }

    const double &max() const { return m_max; }

   private:
    double m_min;
    double m_max;
  };

  /**
   * DimensionIDSet declares a set of DimensionID that defines the ID of a set of dimensions of a polar
   * @tparam _dim the number of dimensions of a polar
   */
  template<size_t _dim>
  class DimensionIDSet {
   public:
    using IDSet = std::array<std::shared_ptr<DimensionID>, _dim>;

    explicit DimensionIDSet(const IDSet &array) :
        m_ID_set(array) {
    }

    bool operator==(const DimensionIDSet<_dim> &other) const {
      for (size_t i = 0; i < _dim; ++i) {
        if (*m_ID_set[i] != *other.m_ID_set[i]) {
          return false;
        }
      }
      return true;
    }

    bool operator!=(const DimensionIDSet<_dim> &other) const {
      return !(*this == other);
    }

    DimensionID *get(size_t i) const { return m_ID_set.at(i).get(); }

    const size_t get_index(const std::string &name) const {
      for (size_t i = 0; i < _dim; ++i) {
        if (m_ID_set.at(i)->name() == name) return i;
      }

      std::string available_names = "{";
      for (const auto &dim_ID: m_ID_set) {
        available_names += dim_ID->name() + "; ";
      }
      available_names += "}";

      spdlog::critical(R"(In DimensionIDSet, attempting to access to non registered dimension name "{}".
                       Available dimensions for this set are {})", name, available_names);
      CRITICAL_ERROR
    }

   private:
    IDSet m_ID_set;

  };

  // Forward declaration
  template<size_t>
  class DimensionPointSet;

  /**
   * A DimensionPoint describes one valued point of a DimensionPointSet. It defines a polar coordinate of dimension _dim
   *
   * It must not be instantiated outside a DimensionPointSet as it needs to retain the DimensionPointSet it belongs to.
   * As a result, we never instantiate it directly but only obtain a DimensionPoint from a DimensionPointSet that has
   * been built.
   *
   * @tparam _dim the dimension of the DimensionPoint (number of components)
   */
  template<size_t _dim>
  class DimensionPoint {
   public:
    using Components = std::array<double, _dim>;

    DimensionPoint(std::shared_ptr<DimensionIDSet<_dim>> dimensionIdSet, const Components &components) :
        m_components(components),
        m_dimensionIdSet(dimensionIdSet) {

      Check();
    }

    const double &get(size_t i) const { return m_components.at(i); }

    const double &get(const std::string &name) {
      return m_components.at(m_dimensionIdSet->get_index(name));
    }

    bool operator<(const DimensionPoint<_dim> &other) const {
      // FIXME: ne fonctionne pas !!! Chercher une autre implementation

//      bool equal = true;
//      for (size_t i = 0; i < _dim; ++i) {
//        if (m_components[i] > other.m_components[i]) return false;
//        if (m_components[i] != other.m_components[i]) equal = false;
//      }
//      return !equal;

      return true;
    }

   private:
    void Check() {
      // Checking if the given values are well between min and max for each dimension
      for (size_t i = 0; i < _dim; ++i) {
        const double val = m_components.at(i);
        DimensionID *ID = m_dimensionIdSet->get(i);
        if (val < ID->min() || val > ID->max()) {
          spdlog::critical("For dimension {}, values {} is out of range [{}, {}]",
                           ID->name(), val, ID->min(), ID->max());
          CRITICAL_ERROR
        }
      }
    }

   private:
    Components m_components;
    std::shared_ptr<DimensionIDSet<_dim>> m_dimensionIdSet;

  };

  template<size_t _dim>
  class DimensionPointGrid;

  struct DimensionPointSetBase {
    virtual ~DimensionPointSetBase() = default;  // To make it polymorphic for usage of std::dynamic_cast in polar reader
  };

  /**
   * A DimensionPointSet represents a set of DimensionPoint built from the cartesian product of value vectors given for
   * each dimension.
   *
   * First this class is instantiated given a DimensionIDSet that define the dimensions.
   * Second, each dimension must be given a value vector
   * Third, the cartesian product of dimensions is pre-processed by calling the *build* method.
   * Note that dimensions have precedence in DimensionIDSet (first, second etc...) and that the last dimension is the
   * one that is moving faster so that the resulting _dim-dimensional arrays are row-major.
   *
   * Iterators are available to iterate on every of the points of the DimensionPointSet
   *
   * @tparam _dim
   */
  template<size_t _dim>
  class DimensionPointSet : public DimensionPointSetBase {
   public:
    using DimensionPointVector = std::vector<std::shared_ptr<DimensionPoint<_dim>>>;
    using Iter = typename DimensionPointVector::const_iterator;

    explicit DimensionPointSet(std::shared_ptr<DimensionIDSet<_dim>> dimension_ID_set) :
//        m_is_built(false),
        m_grid(nullptr),
        m_dimension_ID_set(dimension_ID_set) {}

    DimensionPointSet(std::shared_ptr<DimensionIDSet<_dim>> dimension_ID_set,
                      DimensionPointGrid<_dim> *grid) :
        m_grid(grid),
        m_dimension_ID_set(dimension_ID_set) {}

    const DimensionIDSet<_dim> *dimension_ID_set() const { return m_dimension_ID_set.get(); }

    std::shared_ptr<DimensionIDSet<_dim>> dimension_ID_set() { return m_dimension_ID_set; }

    std::shared_ptr<DimensionPoint<_dim>> at(size_t i) const {
      return m_dimension_points.at(i);
    }

    const DimensionPointGrid<_dim> *grid() const {
      return m_grid;
    }

    std::vector<std::shared_ptr<DimensionPointSet<_dim>>> split(const size_t chunk_size) const {

      size_t nviews = size() / chunk_size;
      size_t rem = size() % chunk_size;

      std::vector<std::shared_ptr<DimensionPointSet<_dim>>> dimension_point_set_vector;

      if (nviews == 0) {
        // We have only one view with rem polar points inside
        dimension_point_set_vector.reserve(1);

        DimensionPointVector dim_point_vector;
        dim_point_vector.reserve(rem);
        for (size_t i = 0; i < rem; ++i) {
          dim_point_vector.push_back(m_dimension_points[i]);
        }

        auto dim_point_set = std::make_shared<DimensionPointSet<_dim>>(m_dimension_ID_set, m_grid);
        dim_point_set->m_dimension_points = dim_point_vector;
//        dim_point_set->m_is_built = true;

        dimension_point_set_vector.push_back(dim_point_set);

      } else {
        // Static balancing of computations

        dimension_point_set_vector.reserve(nviews);

        // Managing cases where rem > nviews. We have to balance more than one element from rem in each view
        size_t s;
        size_t rem2;
        if (rem <= nviews) {
          s = 1;
          rem2 = 0;
        } else {
          s = rem / nviews;
          rem2 = rem % nviews;
        }

        size_t offset = 0;
        size_t size;
        for (size_t iview = 0; iview < nviews; ++iview) {
          size = chunk_size;
          if (rem > 0) {
            size += s;
            rem -= s;
            if (rem2 > 0) {
              size++;
              rem2--;
              rem--;
            }
          }

          DimensionPointVector dimension_points;
          dimension_points.reserve(size);
          for (size_t i = offset; i < offset + size; ++i) {
            dimension_points.push_back(m_dimension_points[i]);
          }

          auto dim_point_set = std::make_shared<DimensionPointSet<_dim>>(m_dimension_ID_set, m_grid);
          dim_point_set->m_dimension_points = dimension_points;

          dimension_point_set_vector.push_back(dim_point_set);

          offset += size;
        }

      }

      return dimension_point_set_vector;
    }

    void append(const DimensionPointSet<_dim> &other) {

      if (*m_dimension_ID_set != *other.dimension_ID_set()) {
        spdlog::critical("Attempting to concatenate polar set of different DimensionIDSet");
        CRITICAL_ERROR
      }

      // Check that we add dimension points in the right order
      if (!(*m_dimension_points.back() < *other.m_dimension_points.front())) {
        spdlog::critical("Attempting to append non increasing dimensions");
        CRITICAL_ERROR
      }

      if (m_grid != other.m_grid) {
        spdlog::critical("Attempting to append polars with DimensionPointSet issued from different grids");
        CRITICAL_ERROR
      }

      for (size_t i = 0; i < other.size(); ++i) {
        m_dimension_points.push_back(other.at(i));
      }

    }

    Iter begin() const { return m_dimension_points.cbegin(); }

    Iter end() const { return m_dimension_points.cend(); }

    const size_t size() const { return m_dimension_points.size(); }

    friend class DimensionPointGrid<_dim>;

   private:
    std::shared_ptr<DimensionIDSet<_dim>> m_dimension_ID_set;
    DimensionPointVector m_dimension_points;
    DimensionPointGrid<_dim> *m_grid;

  };

  template<size_t _dim>
  class DimensionPointGrid {
   public:
    explicit DimensionPointGrid(std::shared_ptr<DimensionIDSet<_dim>> dimension_ID_set) :
        m_is_built(false),
        m_dimension_ID_set(dimension_ID_set) {}

    void set_dimension_values(const std::string &name, const std::vector<double> &values) {
      size_t i = m_dimension_ID_set->get_index(name);
      m_dimension_vectors[i] = values;
      m_is_built = false;
    }

    size_t size() const {
      if (!is_filled()) {
        spdlog::critical("Requesting size of non completely filled DimensionPointGrid");
        CRITICAL_ERROR
      }
      size_t size = 1;
      for (size_t i = 0; i < _dim; ++i) {
        size *= m_dimension_vectors.at(i).size();
      }
      return size;
    }

    std::shared_ptr<DimensionPointSet<_dim>> get_dimension_point_set() {
      if (!m_is_built) build();
      return m_dimension_point_set;
    }

    const std::vector<double> &dimension_vector(size_t i) const {
      return m_dimension_vectors.at(i);
    }

   private:
    using DimensionPointVector = std::vector<std::shared_ptr<DimensionPoint<_dim>>>;

    bool is_filled() const {
      for (const auto &vector: m_dimension_vectors) {
        if (vector.empty()) return false;
      }
      return true;
    }

    /**
     * Build the dimension point vector from vector of values for each dimension. It does the cartesian product of
     * dimensions by making the last dimension move faster (the last dimension is the most inner for loop).
     * Can only be called if every dimension has been filled with value vector. If it is not the case, a runtime exception
     * is thrown.
     * @return
     */
    void build() {

      if (!is_filled()) {
        spdlog::critical(
            "Attempting to generate a DimensionPointSet from cartesian grid before each dimensions being filled");
        CRITICAL_ERROR
      }

      DimensionPointVector dimension_points;
      dimension_points.reserve(size());

      typename DimensionPoint<_dim>::Components values;
      meta_for_loop<_dim>(dimension_points, values);

      m_dimension_point_set = std::make_shared<DimensionPointSet<_dim>>(m_dimension_ID_set, this);
      m_dimension_point_set->m_dimension_points = dimension_points;

      m_is_built = true;
    }

    // Adapted from https://stackoverflow.com/questions/34535795/n-dimensionally-nested-metaloops-with-templates
    // We use row major convention (last dimension varies the fastest) to be directly compliant with NetCDF internal
    // storage convention (and it is C compliant too...)
    template<size_t index>
    constexpr void meta_for_loop(DimensionPointVector &dimension_points,
                                 typename DimensionPoint<_dim>::Components &values) {
      size_t i = _dim - index; // makes the last loop iterate on last dim
      for (const auto &element: m_dimension_vectors[i]) {
        values.at(i) = element;
        if constexpr (index == 1) {
          dimension_points.push_back(std::make_shared<DimensionPoint<_dim>>(m_dimension_ID_set, values));
        } else {
          meta_for_loop<index - 1>(dimension_points, values);
        }
      }
    }


   private:
    mutable bool m_is_built;

    std::shared_ptr<DimensionIDSet<_dim>> m_dimension_ID_set;
    std::array<std::vector<double>, _dim> m_dimension_vectors;

    std::shared_ptr<DimensionPointSet<_dim>> m_dimension_point_set;
  };


}  // poem

#endif //POEM_DIMENSIONS_H
