//
// Created by frongere on 17/12/24.
//

#ifndef POEM_POLARTABLE_H
#define POEM_POLARTABLE_H

#include <string>
#include <poem/exceptions.h>

#include <MathUtils/RegularGridInterpolator.h>

using namespace poem;


namespace poem2 {

  // ===================================================================================================================

  class Dimension {
   public:
    explicit Dimension(const std::string &name) :
        m_name(name) {
    }

    const std::string &name() const { return m_name; }

   private:
    std::string m_name;

  };

  std::shared_ptr<Dimension> make_dimension(const std::string &name) {
    return std::make_shared<Dimension>(name);
  }

  // ===================================================================================================================

  class DimensionSet {
   public:
    using DimensionVector = std::vector<std::shared_ptr<Dimension>>;
//    using DimensionSetIter = DimensionVector::iterator;
    using DimensionSetConstIter = DimensionVector::const_iterator;

    DimensionSet(const DimensionVector dimensions) : m_dimensions(dimensions) {}

    size_t size() const { return m_dimensions.size(); }

    const std::string &name(size_t i) const { return m_dimensions[i]->name(); }

    DimensionSetConstIter begin() const { return m_dimensions.begin(); }

    DimensionSetConstIter end() const { return m_dimensions.end(); }

   private:
    DimensionVector m_dimensions;

  };

  std::shared_ptr<DimensionSet> make_dimension_set(const std::vector<std::shared_ptr<Dimension>> dimensions) {
    return std::make_shared<DimensionSet>(dimensions);
  }

  // ===================================================================================================================
  class DimensionPoint {
    using Values = std::vector<double>;
    using ValuesConstIter = Values::const_iterator;

   public:
    DimensionPoint(const std::shared_ptr<DimensionSet> &dimension_set) :
        m_dimension_set(dimension_set),
        m_values(dimension_set->size()) {}

    DimensionPoint(const std::shared_ptr<DimensionSet> &dimension_set, const std::vector<double> &values) :
        m_dimension_set(dimension_set),
        m_values(values) {}

    size_t size() const {
      return m_dimension_set->size();
    }

    double &operator[](size_t i) { return m_values[i]; }

    const double &operator[](size_t i) const { return m_values[i]; }

    /*
     * Iterators
     */
    ValuesConstIter begin() const { return m_values.cbegin(); }

    ValuesConstIter end() const { return m_values.cend(); }


    std::ostream &cout(std::ostream &os) const {
      for (size_t i = 0; i < m_dimension_set->size(); ++i) {
        os << m_dimension_set->name(i) << ": " << m_values.at(i) << ";\t";
      }
      return os;
    }

   private:
    std::shared_ptr<DimensionSet> m_dimension_set;
    Values m_values;

  };

  std::ostream &operator<<(std::ostream &os, const DimensionPoint &dimension_point) {
    return dimension_point.cout(os);
  }

  // ===================================================================================================================

  class DimensionGrid {
   public:
    explicit DimensionGrid(const std::shared_ptr<DimensionSet> &dimension_set) :
        m_dimension_set(dimension_set),
        m_dimensions_values(dimension_set->size()),
        m_is_initialized(false) {
    }

    void set_values(const std::string &name, const std::vector<double> &values) {
      //TODO check qu'on a des valeurs en ordre ascendant strictement monotone...
      size_t idx = 0;
      for (const auto &dimension: *m_dimension_set) {
        if (dimension->name() == name) {
          m_dimensions_values.at(idx) = values;
          return;
        }
        idx++;
      }

      spdlog::critical("Unknown dimesion name {}", name);
      CRITICAL_ERROR_POEM
    }

    const std::vector<double> &values(size_t idx) const {
      return m_dimensions_values.at(idx);
    }

    size_t size() const {
      return dimension_points().size();
    }

    size_t dim() const {
      return m_dimension_set->size();
    }

    std::shared_ptr<DimensionSet> dimension_set() const { return m_dimension_set; }

    [[nodiscard]] const std::vector<DimensionPoint> &dimension_points() const {
      if (!m_is_initialized) {
        build_dimension_points();
      }
      return m_dimension_points;
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

//      for (const auto &dimension_point_: m_dimension_points) {
//        std::cout << dimension_point_ << std::endl;
//      }


      self->m_is_initialized = true;
//      NIY_POEM

    }

    [[nodiscard]] bool is_filled() const {
      struct IsEmpty {
        bool operator()(const std::vector<double> &values) {
          return !values.empty();
        }
      };
      return std::all_of(m_dimensions_values.begin(), m_dimensions_values.end(), IsEmpty());
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


  // ===================================================================================================================
  // ===================================================================================================================
  // ===================================================================================================================
  // ===================================================================================================================

  // Forward declaration
  template<typename T>
  class PolarTable;

  template<typename T, size_t _dim>
  class Interpolator;

  struct InterpolatorBase {

    virtual void build() = 0;

  };

  template<typename T, size_t _dim>
  class Interpolator : public InterpolatorBase {
   public:
    explicit Interpolator(PolarTable<T> *polar_table) : m_polar_table(polar_table) {}

    T interp(const DimensionPoint &dimension_point, bool bound_check) const {
      std::array<T, _dim> array;
      size_t idx = 0;
      for (const auto &coord: dimension_point) {
        array[idx] = coord;
        idx++;
      }

      m_interpolator->Interp(array, bound_check);
    }

    void build() override {
      m_interpolator = std::make_unique<mathutils::RegularGridInterpolator<T, _dim>>();
      // Check que m_polar_table est filled

      using NDArray = boost::multi_array<double, _dim>;
      using IndexArray = boost::array<typename NDArray::index, _dim>;
      IndexArray shape;

      auto dimension_grid = m_polar_table->dimension_grid();
      for (size_t idim = 0; idim < dimension_grid->dim(); ++idim) {
        auto values = dimension_grid->values(idim);
        shape[idim] = values.size();
        m_interpolator->AddCoord(values);
      }

      NDArray array(shape);
      std::copy(m_polar_table->values().begin(), m_polar_table->values().end(), array.data());
      this->m_interpolator->AddVar(array);

    }


   private:
    PolarTable<T> *m_polar_table;
    std::unique_ptr<mathutils::RegularGridInterpolator<T, _dim>> m_interpolator;
  };


  // ===================================================================================================================

  class PolarTableBase {

  };

  // ===================================================================================================================

//  template<typename T, class = std::enable_if_t<std::is_arithmetic_v<T>>>
  template<typename T>
  class PolarTable : public PolarTableBase {
   public:
    PolarTable(std::shared_ptr<DimensionGrid> dimension_grid) :
        PolarTableBase(),
        m_dimension_grid(dimension_grid),
        m_values(dimension_grid->size()),
        m_is_interpolator_built(false) {

    }

    size_t size() const {
      return m_dimension_grid->size();
    }

    size_t dim() const {
      return m_dimension_grid->dimension_set()->size();
    }

    std::vector<size_t> shape() const {
      std::vector<size_t> shape(dim());
      for (size_t idx=0; idx<dim(); ++idx) {
        shape[idx] = m_dimension_grid->values(idx).size();
      }
      return shape;
    }

    std::shared_ptr<DimensionGrid> dimension_grid() const {
      return m_dimension_grid;
    }

    [[nodiscard]] const std::vector<DimensionPoint> &dimension_points() const {
      return m_dimension_grid->dimension_points();
    }

    void set_value(size_t idx, const T &value) {
      m_values[idx] = value;
      m_is_interpolator_built = false;
    }

    const std::vector<T> &values() const {
      return m_values;
    }

    T nearest() const {
      NIY_POEM
    }

    T interp(const DimensionPoint &dimension_point, bool bound_check) const {
      // TODO
//      std::lock_guard<std::mutex> lock(this->m_mutex);

      if (!m_is_interpolator_built) {
        auto self = const_cast<PolarTable<T> *>(this);
        self->build_interpolator();
        self->m_is_interpolator_built = true;
      }

      T val;
      switch (dim()) {
        case 1:
          val = static_cast<Interpolator<T, 1> *>(m_interpolator.get())->interp(dimension_point, bound_check);
          break;
        case 2:
          val = static_cast<Interpolator<T, 2> *>(m_interpolator.get())->interp(dimension_point, bound_check);
          break;
        case 3:
          val = static_cast<Interpolator<T, 3> *>(m_interpolator.get())->interp(dimension_point, bound_check);
          break;
        default:
          spdlog::critical("ND interpolation not supported for dimensions higher than 3 (found {})", dim());
          CRITICAL_ERROR_POEM
      }

      return val;
    }

   private:
    void build_interpolator() {

      switch (dim()) {
        case 1:
          m_interpolator = std::make_unique<Interpolator<T, 1>>(this);
          break;
        case 2:
          m_interpolator = std::make_unique<Interpolator<T, 2>>(this);
          break;
        case 3:
          m_interpolator = std::make_unique<Interpolator<T, 3>>(this);
          break;
        default:
          spdlog::critical("ND interpolation not supported for dimensions higher than 3 (found {})", dim());
          CRITICAL_ERROR_POEM
      }
      m_interpolator->build();
      m_is_interpolator_built = true;
    }

   private:
    std::shared_ptr<DimensionGrid> m_dimension_grid;
    std::vector<T> m_values;

    bool m_is_interpolator_built;
    std::unique_ptr<InterpolatorBase> m_interpolator;
  };

  template<typename T>
  std::shared_ptr<PolarTable<T>> make_polar_table(std::shared_ptr<DimensionGrid> dimension_grid) {
    return std::make_shared<PolarTable<T>>(dimension_grid);
  }

}

#endif //POEM_POLARTABLE_H
