//
// Created by frongere on 17/12/24.
//

#ifndef POEM_POLARTABLE_H
#define POEM_POLARTABLE_H

#include <string>
#include "boost/multi_array.hpp"
#include "netcdf"
#include <poem2/exceptions.h>

#include "dunits/dunits.h"

#include "MathUtils/RegularGridInterpolator.h"

namespace poem {

  // ===================================================================================================================

  /**
   * Base class with name, unit and description
   */
  class Named {
   public:

    /**
     * CTOR
     * @param name the name of the dimension
     * @param unit the unit used for that dimension
     * @param description described the dimension
     */
    Named(const std::string &name,
          const std::string &unit,
          const std::string &description) :
        m_name(name),
        m_unit(unit),
        m_description(description) {

      check_unit();

      #ifndef ALLOW_DIRTY
      if (git::AnyUncommittedChanges()) {
        spdlog::critical("Using POEM with uncommitted code modifications is forbidden. Current POEM version: {}",
                         git::GetNormalizedVersionString());
        CRITICAL_ERROR_POEM
      }
      #endif
    }

    virtual const std::string &name() const { return m_name; }

    void change_name(const std::string &new_name) { m_name = new_name; }

    const std::string &unit() const { return m_unit; }

    void change_unit(const std::string &new_unit) { m_unit = new_unit; }

    const std::string &description() const { return m_description; }

    void change_description(const std::string &new_description) { m_description = new_description; }

   private:
    void check_unit() {
      if (!dunits::UnitsChecker::getInstance().is_valid_unit(m_unit, true)) {
        spdlog::critical("Unit \"{}\" is not a valid unit as per dunits library.", m_unit);
        CRITICAL_ERROR_POEM
      }
    }

   protected:
    std::string m_name;
    std::string m_unit;
    std::string m_description;

  };

  // ===================================================================================================================

  /**
   * Declares a polar table dimension with name, unit and description
   */
  class Dimension : public Named {
   public:
    Dimension(const std::string &name,
              const std::string &unit,
              const std::string &description) :
        Named(name, unit, description) {}
  };

  /**
   * Helper function to create a shared_ptr of Dimension
   * @param name
   * @param unit
   * @param description
   * @return
   */
  std::shared_ptr<Dimension> make_dimension(const std::string &name,
                                            const std::string &unit,
                                            const std::string &description) {
    return std::make_shared<Dimension>(name, unit, description);
  }

  // ===================================================================================================================

  /**
   * Declares an ordered set of Dimension objects to be used as a basis to define the dimensions of a PolarTable
   */
  class DimensionSet {
   public:
    using DimensionVector = std::vector<std::shared_ptr<Dimension>>;
    using DimensionSetConstIter = DimensionVector::const_iterator;

    explicit DimensionSet(const DimensionVector dimensions) : m_dimensions(dimensions) {
      // Building map (acceleration)
      size_t idx = 0;
      for (const auto &dimension: dimensions) {
        m_map.insert({dimension->name(), idx});
        idx++;
      }
    }

    size_t size() const { return m_dimensions.size(); }

    const std::string &name(size_t i) const { return m_dimensions.at(i)->name(); }

    size_t index(const std::string &name) const {
      return m_map.at(name);
    }

    bool is_dim(const std::string &name) const {
      return m_map.contains(name);
    }

    std::shared_ptr<Dimension> dimension(size_t i) const {
      return m_dimensions.at(i);
    }

    std::shared_ptr<Dimension> dimension(const std::string &name) const {
      return dimension(index(name));
    }

    DimensionSetConstIter begin() const { return m_dimensions.begin(); }

    DimensionSetConstIter end() const { return m_dimensions.end(); }

   private:
    DimensionVector m_dimensions;
    std::unordered_map<std::string, size_t> m_map;

  };

  std::shared_ptr<DimensionSet> make_dimension_set(const std::vector<std::shared_ptr<Dimension>> dimensions) {
    return std::make_shared<DimensionSet>(dimensions);
  }

  // ===================================================================================================================
  /**
   * A particular point corresponding to a DimensionSet
   */
  class DimensionPoint {
    using Values = std::vector<double>;
    using ValuesConstIter = Values::const_iterator;

   public:
    explicit DimensionPoint(const std::shared_ptr<DimensionSet> &dimension_set) :
        m_dimension_set(dimension_set),
        m_values(dimension_set->size()) {}

    DimensionPoint(const std::shared_ptr<DimensionSet> &dimension_set, const std::vector<double> &values) :
        m_dimension_set(dimension_set),
        m_values(values) {}

    std::shared_ptr<DimensionSet> dimension_set() const {
      return m_dimension_set;
    }

    size_t size() const {
      return m_dimension_set->size();
    }

    double &operator[](size_t i) { return m_values[i]; }

    const double &operator[](size_t i) const { return m_values[i]; }

    void operator=(const std::vector<double> &values) {
      if (values.size() != m_values.size()) {
        spdlog::critical("Attempt to fill a DimensionPoint with bad vector size ({} and {})",
                         values.size(), m_values.size());
        CRITICAL_ERROR_POEM
      }
      m_values = values;
    }

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

      if (!m_dimension_set->is_dim(name)) {
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

  // ===================================================================================================================

  // Forward declaration
  template<typename T>
  class PolarTable;

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

      return m_interpolator->Interp(array, bound_check);
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

  /**
   * PolarTable datatype
   */
  enum POEM_DATATYPE {
    /// double
    POEM_DOUBLE,
    /// int
    POEM_INT,
  };

  // ===================================================================================================================

  struct PolarTableBase {
    virtual POEM_DATATYPE type() const = 0;

    virtual const std::string &name() const = 0;
  };

  /**
   * A multidimensional numerical table representing a variable
   *
   * @tparam T the datatype of the data into the PolarTable
   */
  template<typename T>
  class PolarTable : public PolarTableBase, public Named, public std::enable_shared_from_this<PolarTable<T>> {

   public:
    /**
     * Constructor
     *
     * @param name name of the table
     * @param unit unit of the table
     * @param description description of the table
     * @param type datatype of the table
     * @param dimension_grid the grid of the table
     */
    PolarTable(const std::string &name,
               const std::string &unit,
               const std::string &description,
               POEM_DATATYPE type,
               std::shared_ptr<DimensionGrid> dimension_grid) :
        Named(name, unit, description),
        m_type(type),
        m_dimension_grid(dimension_grid),
        m_values(dimension_grid->size()),
        m_interpolator(nullptr) {

    }

    /**
     * Get the name of the table
     */
    [[nodiscard]] const std::string &name() const override { return Named::m_name; }

    /**
     * Get the type of the table
     */
    [[nodiscard]] POEM_DATATYPE type() const override { return m_type; }

    /**
     * Number of points in the table (equal to the number of DimensionPoint in the Grid)
     */
    [[nodiscard]] size_t size() const {
      return m_dimension_grid->size();
    }

    /**
     * Number of points along the dimension with index idx
     *
     * It is equal to the number of elements of the grid for that dimension
     * @param idx index of the dimension
     */
    [[nodiscard]] size_t size(size_t idx) const {
      return m_dimension_grid->size(idx);
    }

    /**
     * Number of dimensions of the table
     *
     * Also the size of the associated DimensionSet
     */
    [[nodiscard]] size_t dim() const {
      return m_dimension_grid->dimension_set()->size();
    }

    /**
     * Get a vector of sizes for the dimensions of the table
     *
     * Also the shape of the associated DimensionGrid
     */
    [[nodiscard]] std::vector<size_t> shape() const {
      return m_dimension_grid->shape();
    }

    /**
     * Get the associated DimensionGrid
     */
    [[nodiscard]] std::shared_ptr<DimensionGrid> dimension_grid() const {
      return m_dimension_grid;
    }

    /**
     * Get the vector of DimensionPoint corresponding to the associated DimensionGrid of the table
     */
    [[nodiscard]] const std::vector<DimensionPoint> &dimension_points() const {
      return m_dimension_grid->dimension_points();
    }

    /**
     * Set a particular value of the table at index idx corresponding to the DimensionPoint that you can get from
     * dimension_points()
     *
     * @param idx index of the value to set
     * @param value value to set
     */
    void set_value(size_t idx, const T &value) {
      m_values[idx] = value;
      reset();
    }

    /**
     * Get the whole data vector of the table.
     */
    [[nodiscard]] const std::vector<T> &values() const {
      return m_values;
    }

    /**
     * Get the whole data vector of the table.
     */
    std::vector<T> &values() {
      return m_values;
    }

    /**
     * Set the whole data vector of the table
     * @param new_values vector of values
     */
    void set_values(const std::vector<T> &new_values) {
      if (new_values.size() != m_values.size()) {
        spdlog::critical("Attempting to set values in PolarTable of different size ({} and {})",
                         m_values.size(), new_values.size());
        CRITICAL_ERROR_POEM
      }
      m_values = new_values;
    }

    /**
     * Makes a copy of the current PolarTable (shared_ptr)
     *
     * DimensionGrid is kept shared but data vector is copied
     */
    [[nodiscard]] std::shared_ptr<PolarTable<T>> copy() const {
      auto polar_table = std::make_shared<PolarTable<T>>(m_name, m_unit, m_description, m_type, m_dimension_grid);
      polar_table->set_values(m_values);
      return polar_table;
    }

    /**
     * Multiplies the data of the table by a scalar
     */
    void multiply_by(const T &coeff) {
      for (auto &val: m_values) {
        val *= coeff;
      }
    }

    /**
     * Adds a scalar offset to the data
     */
    void offset(const T &val) {
      for (auto &val_: m_values) {
        val_ += val;
      }
    }

    /**
     * Sums two tables
     */
    void sum(std::shared_ptr<PolarTable<T>> other) {

      if (other->dimension_grid()->dimension_set() != m_dimension_grid->dimension_set()) {
        spdlog::critical("[PolarTable::interp] DimensionPoint has not the same DimensionSet as the PolarTable");
        CRITICAL_ERROR_POEM
      }

      if (other->size() != size()) {
        spdlog::critical("Attempting to sum two PolarTable of different size ({} and {}", size(), other->size());
        CRITICAL_ERROR_POEM
      }
      for (size_t idx = 0; idx < size(); ++idx) {
        m_values[idx] += other->m_values[idx];
      }
    }

    /**
     * Takes the absolute value of the data
     */
    void abs() {
      for (auto &val: m_values) {
        val = std::abs(val);
      }
    }

    /**
     * Calculates the mean of the table
     * @return
     */
    [[nodiscard]] T mean() const {
      T mean = 0;
      for (const auto &val: m_values) {
        mean += val;
      }
      return mean / (T) size();
    }

    /**
     * Get the value corresponding to the nearest DimensionPoint from given dimension_point in the associated
     * DimensionGrid
     */
    [[nodiscard]] T nearest(const DimensionPoint &dimension_point) const {

      if (dimension_point.dimension_set() != m_dimension_grid->dimension_set()) {
        spdlog::critical("[PolarTable::nearest] DimensionPoint has not the same DimensionSet as the PolarTable");
        CRITICAL_ERROR_POEM
      }

      std::vector<size_t> indices(dim());
      size_t index = 0;
      for (const auto &coord: dimension_point) {
        if (coord < m_dimension_grid->min(index) || coord > m_dimension_grid->max(index)) {
          spdlog::critical("[PolarTable::nearest] dimension_point is out of bound of DimensionGrid");
          CRITICAL_ERROR_POEM
        }
        auto values = m_dimension_grid->values(index);
        auto it = std::min_element(values.begin(), values.end(),
                                   [coord](double a, double b) {
                                     return std::abs(a - coord) < std::abs(b - coord);
                                   });

        indices[index] = std::distance(values.begin(), it);
        index++;
      }

      index = m_dimension_grid->grid_to_index(indices);

      return m_values[index];
    }

    /**
     * Get the value of the interpolation to dimension_point
     */
    [[nodiscard]] T interp(const DimensionPoint &dimension_point, bool bound_check) const {
//      std::lock_guard<std::mutex> lock(this->m_mutex);

      if (dimension_point.dimension_set() != m_dimension_grid->dimension_set()) {
        spdlog::critical("[PolarTable::interp] DimensionPoint has not the same DimensionSet as the PolarTable");
        CRITICAL_ERROR_POEM
      }

      if (m_type != POEM_DOUBLE) {
        spdlog::critical("Attempting to interpolate on a non-floating point PolarTable");
        CRITICAL_ERROR_POEM
      }

      if (!m_interpolator) {
        const_cast<PolarTable<T> *>(this)->build_interpolator();
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
        case 4:
          val = static_cast<Interpolator<T, 4> *>(m_interpolator.get())->interp(dimension_point, bound_check);
          break;
        case 5:
          val = static_cast<Interpolator<T, 5> *>(m_interpolator.get())->interp(dimension_point, bound_check);
          break;
        case 6:
          val = static_cast<Interpolator<T, 6> *>(m_interpolator.get())->interp(dimension_point, bound_check);
          break;
        default:
          spdlog::critical("ND interpolation not supported for dimensions higher than 6 (found {})", dim());
          CRITICAL_ERROR_POEM
      }

      return val;
    }

    /**
     * Get a slice in the table given values for different dimensions
     *
     * prescribed_values is a std::unordered_map whose key is the name of a dimension (must be a valid Dimension name
     * of the associated DimensionSet) and value is the prescribed value for this dimension
     */
    [[nodiscard]] std::shared_ptr<PolarTable<T>>
    slice(std::unordered_map<std::string, double> prescribed_values) const {

      // Check that names are existing
      auto dimension_set = m_dimension_grid->dimension_set();
      for (const auto &pair: prescribed_values) {
        if (!dimension_set->is_dim(pair.first)) {
          spdlog::critical("Slicing PolarTable \"{}\" with unknown dimension name {}", m_name, pair.first);
          CRITICAL_ERROR_POEM
        }
        // TODO: tester ranges des valeurs

      }

      // Building a new grid
      auto new_dimension_grid = std::make_shared<DimensionGrid>(dimension_set);
      for (const auto &dimension: *dimension_set) {
        auto dimension_name = dimension->name();

        if (prescribed_values.contains(dimension_name)) {
//          std::cout << "Slicing with " << dimension_name << " = " << prescribed_values.at(dimension_name) << std::endl;
          new_dimension_grid->set_values(dimension_name, {prescribed_values.at(dimension_name)});
        } else {
//          std::cout << "Keep sampling on " << dimension_name << std::endl;
          new_dimension_grid->set_values(dimension_name, m_dimension_grid->values(dimension_name));
        }

      }

      // New polar table
      auto sliced_polar_table = std::make_shared<PolarTable<T>>(m_name, m_unit, m_description, m_type,
                                                                new_dimension_grid);

      // Interpolation on every DimensionPoint
      size_t idx = 0;
      for (const auto &dimension_point: new_dimension_grid->dimension_points()) {
        T val = interp(dimension_point, false); // Bound checking already done above
//        std::cout << dimension_point << " -> " << val << std::endl;
        sliced_polar_table->set_value(idx, val);
        idx++;
      }

      return sliced_polar_table;
    }

    /**
     * Removes Dimension if it is a singleton (only one associated value in the associated DimensionGrid for a Dimension)
     *
     * Inline version
     *
     * @return true if the dimension has been reduced, false otherwise
     */
    bool squeeze() {

      // Number of dimensions to squeeze
      size_t n = dim();
      for (size_t i = 0; i < dim(); ++i) {
        if (size(i) == 1) n -= 1;
      }

      if (n == 0) {
        // No dimension to squeeze, return
        return false;
      }

      // Create a new reduced DimensionSet
      std::vector<std::shared_ptr<Dimension>> new_dimensions;
      new_dimensions.reserve(n);
      for (size_t i = 0; i < dim(); ++i) {
        if (size(i) > 1) {
          new_dimensions.push_back(m_dimension_grid->dimension_set()->dimension(i));
        }
      }

      auto new_dimension_set = make_dimension_set(new_dimensions);
      auto new_dimension_grid = make_dimension_grid(new_dimension_set);

      // Put the values into the new DimensionGrid
      for (const auto &dimension: *new_dimension_set) {
        auto name = dimension->name();
        new_dimension_grid->set_values(name, m_dimension_grid->values(name));
      }

      m_dimension_grid = new_dimension_grid;

      reset();

      return true;
    }

    /**
     * Removes Dimension if it is a singleton (only one associated value in the associated DimensionGrid for a Dimension)
     *
     * @return a copy of the PolarTable with fewer dimensions if some of the dimensions were singleton, otherwise
     * returns shared_ptr of itself
     */
    [[nodiscard]] std::shared_ptr<PolarTable<T>> squeeze() const {
      std::shared_ptr<PolarTable<T>> polar_table;

      auto polar_table_ = copy();
      if (polar_table_->squeeze()) {
        polar_table = polar_table_;
      } else {
        polar_table = this->shared_from_this();
      }
      return polar_table;
    }

    [[nodiscard]] std::shared_ptr<PolarTable<T>> resample(std::shared_ptr<DimensionGrid> new_dimension_grid) const {

      if (new_dimension_grid->dimension_set() != m_dimension_grid->dimension_set()) {
        spdlog::critical("[PolarTable::resample] DimensionGrid has not the same DimensionSet as the PolarTable");
        CRITICAL_ERROR_POEM
      }

      if (new_dimension_grid->dim() != dim()) {
        spdlog::critical("Dimension mismatch in resampling operation ({} and {})", new_dimension_grid->dim(), dim());
        CRITICAL_ERROR_POEM
      }

      if (!new_dimension_grid->is_filled()) {
        spdlog::critical("DimensionGrid for resampling is not filled");
        CRITICAL_ERROR_POEM
      }

      for (size_t idim = 0; idim < dim(); ++idim) {
        if (new_dimension_grid->min(idim) < m_dimension_grid->min(idim) ||
            new_dimension_grid->max(idim) > m_dimension_grid->max(idim)) {
          spdlog::critical("Out of range values for resampling");
          CRITICAL_ERROR_POEM
        }
      }

      auto resampled_polar_table = std::make_shared<PolarTable<T>>(m_name, m_unit, m_description, m_type,
                                                                   new_dimension_grid);

      size_t idx = 0;
      for (const auto &dimension_point: new_dimension_grid->dimension_points()) {
        T interp_value = interp(dimension_point, false);
        resampled_polar_table->set_value(idx, interp_value);
        idx++;
      }

      return resampled_polar_table;
    }

//    /**
//     * Writes the table into a NetCDF group
//     */
//    void to_netcdf(netCDF::NcGroup &group) const {
//
//      // TODO: check qu'on va ecrire une polaire qui est bien remplie
//
////      auto grid = m_dimension_point_set->dimension_grid(); // FIXME: dimension_grid est vide;..
//      auto dimension_set = m_dimension_grid->dimension_set();
//
//      // Storing dimensions
//      std::vector<netCDF::NcDim> dims;
//      dims.reserve(dim());
//
//      for (size_t i = 0; i < dim(); ++i) {
////        m_dimension_grid->
//        auto dimension = dimension_set->dimension(i);
//        auto name = dimension->name();
//        auto values = m_dimension_grid->values(i);
//
//        // TODO: voir si on eut pas detecter que le nom est deja pris...
//        // Declaration of a new dimension ID
//        auto dim = group.getDim(name);
//        if (dim.isNull()) {
//          dim = group.addDim(name, values.size());
//
//          // The dimension as a variable
//          netCDF::NcVar nc_var = group.addVar(name, netCDF::ncDouble, dim);
//          nc_var.setCompression(true, true, 5);
//          nc_var.putVar(values.data());
//          /*
//           * FIXME: les attributs ici sont completement decorreles du schema...
//           *  il faudrait ajouter ces champs dynamiquement en amont et les stocker dans un vecteur
//           */
//
//          nc_var.putAtt("unit", dimension->unit());
//          nc_var.putAtt("description", dimension->description());
//        }
//
//        dims.push_back(dim);
//
//      }
//
//      // Storing the values
//      netCDF::NcVar nc_var = group.getVar(Named::name());
//
//      if (nc_var.isNull()) {
//
//        switch (m_type) {
//          case POEM_DOUBLE:
//            nc_var = group.addVar(Named::name(), netCDF::ncDouble, dims);
//            break;
//          case POEM_INT:
//            nc_var = group.addVar(Named::name(), netCDF::ncInt, dims);
//
//        }
//        nc_var.setCompression(true, true, 5);
//
//        nc_var.putVar(m_values.data());
//        nc_var.putAtt("unit", unit());
//        nc_var.putAtt("description", description());
//
//      } else {
//        spdlog::critical("Attempting to store more than one time a variable with the same name {}", Named::name());
//        CRITICAL_ERROR_POEM
//      }
//
//    }

   private:
    void reset() {
      m_interpolator.reset();
    }

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
        case 4:
          m_interpolator = std::make_unique<Interpolator<T, 4>>(this);
          break;
        case 5:
          m_interpolator = std::make_unique<Interpolator<T, 5>>(this);
          break;
        case 6:
          m_interpolator = std::make_unique<Interpolator<T, 6>>(this);
          break;
        default:
          spdlog::critical("ND interpolation not supported for dimensions higher than 6 (found {})", dim());
          CRITICAL_ERROR_POEM
      }
      m_interpolator->build();
    }

   private:
    POEM_DATATYPE m_type;

    std::shared_ptr<DimensionGrid> m_dimension_grid;
    std::vector<T> m_values;

    std::unique_ptr<InterpolatorBase> m_interpolator;
  };

  template<typename T>
  std::shared_ptr<PolarTable<T>> make_polar_table(const std::string &name,
                                                  const std::string &unit,
                                                  const std::string &description,
                                                  POEM_DATATYPE type,
                                                  const std::shared_ptr<DimensionGrid> &dimension_grid) {
    return std::make_shared<PolarTable<T>>(name, unit, description, type, dimension_grid);
  }

  template<typename T>
  void write(netCDF::NcGroup &group, std::shared_ptr<PolarTable<T>> polar_table) {

    auto dimension_grid = polar_table->dimension_grid();
    auto dimension_set = dimension_grid->dimension_set();

    // Storing dimensions
    size_t ndim = polar_table->dim();
    std::vector<netCDF::NcDim> dims;
    dims.reserve(ndim);

    for (size_t i = 0; i < ndim; ++i) {
      auto dimension = dimension_set->dimension(i);
      auto name = dimension->name();
      auto values = dimension_grid->values(i);

      auto dim = group.getDim(name);
      if (dim.isNull()) {
        dim = group.addDim(name, values.size());

        // The dimension as a variable
        netCDF::NcVar nc_var = group.addVar(name, netCDF::ncDouble, dim);
        nc_var.setCompression(true, true, 5);
        nc_var.putVar(values.data());

        nc_var.putAtt("unit", dimension->unit());
        nc_var.putAtt("description", dimension->description());
      }

      dims.push_back(dim);

    }

    // Storing the values
    auto polar_name = polar_table->name();
    netCDF::NcVar nc_var = group.getVar(polar_name);

    if (nc_var.isNull()) {

      switch (polar_table->type()) {
        case POEM_DOUBLE:
          nc_var = group.addVar(polar_name, netCDF::ncDouble, dims);
          break;
        case POEM_INT:
          nc_var = group.addVar(polar_name, netCDF::ncInt, dims);

      }
      nc_var.setCompression(true, true, 5);

      nc_var.putVar(polar_table->values().data());
      nc_var.putAtt("unit", polar_table->unit());
      nc_var.putAtt("description", polar_table->description());

    } else {
      spdlog::critical("Attempting to store more than one time a variable with the same name {}", polar_name);
      CRITICAL_ERROR_POEM
    }
  }

  std::shared_ptr<DimensionGrid> read_dimension_grid(const netCDF::NcVar &var) {

    size_t ndim = var.getDimCount();

    auto group = var.getParentGroup();

    std::vector<std::shared_ptr<Dimension>> dimensions(ndim);
    for (size_t idim = 0; idim < ndim; ++idim) {
      auto dim = var.getDim((int) idim);
      std::string name = dim.getName();
      auto dim_var = group.getVar(name);
      std::string unit, description;
      dim_var.getAtt("unit").getValues(unit);
      dim_var.getAtt("description").getValues(description);

      dimensions[idim] = std::make_shared<Dimension>(name, unit, description);
    }

    auto dimension_set = make_dimension_set(dimensions);
    auto dimension_grid = make_dimension_grid(dimension_set);

    for (size_t idim = 0; idim < ndim; ++idim) {
      auto name = dimension_set->dimension(idim)->name();
      auto dim_var = group.getVar(name);
      std::vector<double> values(dim_var.getDim(0).getSize());
      dim_var.getVar(values.data());

      dimension_grid->set_values(name, values);

    }

    return dimension_grid;
  }

  std::shared_ptr<PolarTableBase>
  read(const netCDF::NcVar &var, std::shared_ptr<DimensionGrid> &dimension_grid, bool build_from_var) {

    if (build_from_var) {
      dimension_grid = read_dimension_grid(var);
    }

    if (var.getDimCount() != dimension_grid->dim()) {
      spdlog::critical("Dimension mismatch between netCDF dataset var and DimensionGrid");
      CRITICAL_ERROR_POEM
    }

    size_t ndim = dimension_grid->dim();
    size_t size = 0;

    std::vector<std::shared_ptr<Dimension>> dimensions(ndim);
    for (size_t idim = 0; idim < ndim; ++idim) {
      if (var.getDim((int) idim).getName() != dimension_grid->dimension_set()->dimension(idim)->name()) {
        spdlog::critical("Dimension name mismatch between netCDF dataset and DimensionGrid");
        CRITICAL_ERROR_POEM
      }
    }

    std::string name = var.getName();
    std::string unit;
    var.getAtt("unit").getValues(unit);
    std::string description;
    var.getAtt("description").getValues(description);

    std::shared_ptr<PolarTableBase> polar_table;

    switch (var.getType().getTypeClass()) {
      case netCDF::NcType::nc_DOUBLE: {
        auto polar_table_ = make_polar_table<double>(name, unit, description, POEM_DOUBLE, dimension_grid);
        var.getVar(polar_table_->values().data());
        polar_table = polar_table_;
        break;
      }
      case netCDF::NcType::nc_INT:
        polar_table = make_polar_table<int>(name, unit, description, POEM_INT, dimension_grid);
        break;
      default:
        spdlog::critical("Cannot read netCDF variable of type {}", var.getType().getName());
        CRITICAL_ERROR_POEM
    }

    return polar_table;

  }

  // ===================================================================================================================

  /**
   * Control type of a Polar
   */
  enum CONTROL_TYPE {
    /// Velocity control
    VELOCITY_CONTROL,
    /// Power control
    POWER_CONTROL,
    /// No control
    NO_CONTROL
  };

  /**
   * The different types of available polar modes in poem
   */
  enum POLAR_MODE {
    /// Motor Power Prediction Program (motor propulsion only, VELOCITY_CONTROL)
    MPPP,
    /// Hybrid Power Prediction Program (motor and wind propulsion, VELOCITY_CONTROL)
    HPPP,
    /// Motor Velocity Prediction Program (motor propulsion only, POWER_CONTROL)
    MVPP,
    /// Hybrid Velocity Prediction Program (motor and wind propulsion, POWER_CONTROL)
    HVPP,
    /// Velocity Prediction Program (wind propulsion only, NO_CONTROL)
    VPP
  };

  /**
   * Get the control type from POLAR_MODE
   */
  CONTROL_TYPE control_type (POLAR_MODE polar_mode) {
    CONTROL_TYPE control_type;
    switch (polar_mode) {
      case MPPP:
        control_type = VELOCITY_CONTROL;
        break;
      case HPPP:
        control_type = VELOCITY_CONTROL;
        break;
      case MVPP:
        control_type = POWER_CONTROL;
        break;
      case HVPP:
        control_type = POWER_CONTROL;
        break;
      case VPP:
        control_type = NO_CONTROL;
        break;
    }
    return control_type;
  }

  /**
   * Converts a string representation into its corresponding POLAR_MODE
   */
  inline POLAR_MODE string_to_polar_mode(const std::string &polar_type_) {
    POLAR_MODE polar_mode;
    if (polar_type_ == "MPPP") {
      polar_mode = MPPP;
    } else if (polar_type_ == "HPPP") {
      polar_mode = HPPP;
    } else if (polar_type_ == "HVPP") {
      polar_mode = HVPP;
    } else if (polar_type_ == "MVPP") {
      polar_mode = MVPP;
    } else if (polar_type_ == "VPP") {
      polar_mode = VPP;
    } else if (polar_type_ == "ND") {
      // This is the v0 version of POEM specs...
      polar_mode = HPPP;
    } else if (polar_type_ == "/") {
      // This is the root group and might be a v0 version of POEM specs...
      polar_mode = HPPP;
    } else {
      spdlog::critical("Polar type \"{}\" unknown", polar_mode);
      CRITICAL_ERROR_POEM
    }
    return polar_mode;
  }

  /**
   * Converts a POLAR_MODE into its corresponding string representation
   */
  inline std::string polar_mode_to_string(const POLAR_MODE &polar_mode_) {
    std::string polar_type;
    switch (polar_mode_) {
      case MPPP:
        polar_type = "MPPP";
        break;
      case HPPP:
        polar_type = "HPPP";
        break;
      case HVPP:
        polar_type = "HVPP";
        break;
      case MVPP:
        polar_type = "MVPP";
        break;
      case VPP:
        polar_type = "VPP";
        break;
    }
    return polar_type;
  }

  // ===================================================================================================================

  /**
   * A Polar stacks the PolarTable for one POLAR_MODE
   *
   * Every PolarTable in a Polar share the same DimensionGrid
   */
  class Polar {
   public:
    Polar(const std::string &name,
          POLAR_MODE mode,
          std::shared_ptr<DimensionGrid> dimension_grid) :
        m_name(name), m_mode(mode), m_dimension_grid(dimension_grid) {

    }

    const std::string &name() const { return m_name; }

    const POLAR_MODE &mode() const { return m_mode; }

    std::shared_ptr<DimensionGrid> dimension_grid() const { return m_dimension_grid; }

    template<typename T>
    std::shared_ptr<PolarTable<T>> new_polar_table(const std::string &name,
                                                   const std::string &unit,
                                                   const std::string &description,
                                                   const POEM_DATATYPE type) {

      auto polar_table = make_polar_table<T>(name, unit, description, type, m_dimension_grid);
      m_polar_tables.insert({name, polar_table});
      return polar_table;
    }

    void add_polar(std::shared_ptr<PolarTableBase> polar_table) {
      m_polar_tables.insert({polar_table->name(), polar_table});
    }

    std::shared_ptr<PolarTableBase> polar(const std::string &name) const {
      return m_polar_tables.at(name);
    }


   private:
    std::string m_name;
    POLAR_MODE m_mode;
    std::shared_ptr<DimensionGrid> m_dimension_grid;
    std::unordered_map<std::string, std::shared_ptr<PolarTableBase>> m_polar_tables;

  };

  std::shared_ptr<Polar> make_polar(const std::string &name,
                                    POLAR_MODE mode,
                                    std::shared_ptr<DimensionGrid> dimension_grid) {
    return std::make_shared<Polar>(name, mode, dimension_grid);
  }

  // ===================================================================================================================

  /**
   * A PolarSet stacks the different Polar for one OperationMode
   */
  class PolarSet {
   public:
    explicit PolarSet(const std::string &name) : m_name(name) {}

    std::shared_ptr<Polar> crete_polar(POLAR_MODE mode,
                                       std::shared_ptr<DimensionGrid> dimension_grid) {

      std::string polar_name = m_name + "." + polar_mode_to_string(mode);
      if (has_polar(mode)) {
        spdlog::warn("In PolarSet {}, Polar {} already exists", m_name, polar_name);
        return m_polars[mode];
      }

      auto polar = std::make_shared<Polar>(polar_name, mode, dimension_grid);
      m_polars.insert({mode, polar});
      return polar;
    }

    bool has_polar(POLAR_MODE mode) { return m_polars.contains(mode); }

    std::shared_ptr<Polar> polar(POLAR_MODE mode) {
      if (!has_polar(mode)) {
        spdlog::critical("PolarSet {} has no Polar of type {}", m_name, polar_mode_to_string(mode));
        CRITICAL_ERROR_POEM
      }
      return m_polars[mode];
    }

   private:
    std::string m_name;
    std::unordered_map<POLAR_MODE, std::shared_ptr<Polar>> m_polars;

  };


} // namespace poem

#endif //POEM_POLARTABLE_H
