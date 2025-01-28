//
// Created by frongere on 17/12/24.
//

#ifndef POEM_POLARTABLE_H
#define POEM_POLARTABLE_H

#include <string>

#include <boost/multi_array.hpp>
#include <MathUtils/RegularGridInterpolator.h>

#include "Dimension.h"
#include "DimensionPoint.h"
#include "DimensionGrid.h"
#include "Named.h"
#include "PolarNode.h"


namespace poem {

  // Forward declaration
  template<typename T>
  class PolarTable;

  /**
   * Non template base class for Interpolator class used by PolarTable
   */
  struct InterpolatorBase {
    virtual ~InterpolatorBase() {}

    virtual void build() = 0;
  };

  /**
   * Multidimensional interpolation class
   * @tparam T the datatype of the interpolation
   * @tparam _dim the number of dimension of the PolarTable
   */
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
      // TODO: Check que m_polar_table est filled

      using NDArray = boost::multi_array<double, _dim>;
      using IndexArray = boost::array<typename NDArray::index, _dim>;
      IndexArray shape;

      auto dimension_grid = m_polar_table->dimension_grid();
      for (size_t idim = 0; idim < dimension_grid->ndims(); ++idim) {
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
  // ===================================================================================================================

  // Forward declaration
  class Polar;

  class DimensionGrid;


  /**
   * Non template base class for PolarTable
   */
  struct PolarTableBase : public PolarNode, public Named {

    PolarTableBase(const std::string &name,
                   const std::string &unit,
                   const std::string &description,
                   POEM_DATATYPE type,
                   std::shared_ptr<DimensionGrid> dimension_grid) :
        PolarNode(name),
        Named(unit, description),
        m_type(type),
        m_dimension_grid(dimension_grid),
        m_interpolator(nullptr) {
      m_polar_node_type = POLAR_TABLE;
    }

    virtual POEM_DATATYPE type() const = 0;

    virtual std::shared_ptr<DimensionGrid> dimension_grid() const = 0;

    virtual bool operator==(const PolarTableBase &other) const = 0;

    virtual bool operator!=(const PolarTableBase &other) const = 0;

    std::shared_ptr<PolarTable<double>> as_polar_table_double() {
      if (m_type != POEM_DOUBLE) {
        LogCriticalError("PolarTable {} has no type double", m_name);
        CRITICAL_ERROR_POEM
      }
      return std::dynamic_pointer_cast<PolarTable<double>>(shared_from_this());
    }

    std::shared_ptr<PolarTable<int>> as_polar_table_int() {
      if (m_type != POEM_INT) {
        LogCriticalError("PolarTable {} has no type int", m_name);
        CRITICAL_ERROR_POEM
      }
      return std::dynamic_pointer_cast<PolarTable<int>>(shared_from_this());
    }

   protected:
    POEM_DATATYPE m_type;
    std::shared_ptr<DimensionGrid> m_dimension_grid;
    std::unique_ptr<InterpolatorBase> m_interpolator;

  };


  /**
   * A multidimensional numerical table representing a variable
   *
   * @tparam T the datatype of the data into the PolarTable
   */
  template<typename T>
  class PolarTable : public PolarTableBase {

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
               std::shared_ptr<DimensionGrid> dimension_grid);

    /**
     * Get the type of the table
     */
    [[nodiscard]] POEM_DATATYPE type() const override;

    /**
     * Number of points in the table (equal to the number of DimensionPoint in the Grid)
     */
    [[nodiscard]] size_t size() const;

    /**
     * Number of points along the dimension with index idx
     *
     * It is equal to the number of elements of the grid for that dimension
     * @param idx index of the dimension
     */
    [[nodiscard]] size_t size(size_t idx) const;

    /**
     * Number of dimensions of the table
     *
     * Also the size of the associated DimensionSet
     */
    [[nodiscard]] size_t dim() const;

    /**
     * Get a vector of sizes for the dimensions of the table
     *
     * Also the shape of the associated DimensionGrid
     */
    [[nodiscard]] std::vector<size_t> shape() const;

    /**
     * Get the associated DimensionGrid
     */
    [[nodiscard]] std::shared_ptr<DimensionGrid> dimension_grid() const override;

    /**
     * Get the vector of DimensionPoint corresponding to the associated DimensionGrid of the table
     */
    [[nodiscard]] const std::vector<DimensionPoint> &dimension_points() const;

    /**
     * Set a particular value of the table at index idx corresponding to the DimensionPoint that you can get from
     * dimension_points()
     *
     * @param idx index of the value to set
     * @param value value to set
     */
    void set_value(size_t idx, const T &value);

    /**
     * Get the whole data vector of the table.
     */
    [[nodiscard]] const std::vector<T> &values() const;

    /**
     * Get the whole data vector of the table.
     */
    std::vector<T> &values();

    /**
     * Set the whole data vector of the table
     * @param new_values vector of values
     */
    void set_values(const std::vector<T> &new_values);

    /**
     * Fill the table with the given value
     * @param value
     */
    void fill_with(T value);

    /**
     * Makes a copy of the current PolarTable (shared_ptr)
     *
     * DimensionGrid is kept shared but data vector is copied
     */
    [[nodiscard]] std::shared_ptr<PolarTable<T>> copy() const;

    /**
     * Multiplies the data of the table by a scalar
     */
    void multiply_by(const T &coeff);

    /**
     * Adds a scalar offset to the data
     */
    void offset(const T &val);

    /**
     * Sums two tables
     */
    void sum(std::shared_ptr<PolarTable<T>> other);

    /**
     * Takes the absolute value of the data
     */
    void abs();

    /**
     * Calculates the mean of the table
     * @return
     */
    [[nodiscard]] T mean() const;

    /**
     * Operator ==
     *
     * Equality is tested on values of DimensionGrid and value vector of the table, not the address of the DimensionGrid
     */
    bool operator==(const PolarTableBase &other) const override;

    /**
     * Operator !=
     *
     * Equality is tested on values of DimensionGrid and value vector of the table, not the address of the DimensionGrid
     */
    bool operator!=(const PolarTableBase &other) const override;

    /**
     * Get the value corresponding to the nearest DimensionPoint from given dimension_point in the associated
     * DimensionGrid
     */
    [[nodiscard]] T nearest(const DimensionPoint &dimension_point) const;

    /**
     * Get the value of the interpolation to dimension_point
     */
    [[nodiscard]] T interp(const DimensionPoint &dimension_point, bool bound_check) const;

    /**
     * Get a slice in the table given values for different dimensions
     *
     * prescribed_values is a std::unordered_map whose key is the name of a dimension (must be a valid Dimension name
     * of the associated DimensionSet) and value is the prescribed value for this dimension
     */
    [[nodiscard]] std::shared_ptr<PolarTable<T>>
    slice(std::unordered_map<std::string, double> prescribed_values) const;

    /**
     * Removes Dimension if it is a singleton (only one associated value in the associated DimensionGrid for a Dimension)
     *
     * Inline version
     *
     * @return true if the dimension has been reduced, false otherwise
     */
    bool squeeze();

    /**
     * Removes Dimension if it is a singleton (only one associated value in the associated DimensionGrid for a Dimension)
     *
     * @return a copy of the PolarTable with fewer dimensions if some of the dimensions were singleton, otherwise
     * returns shared_ptr of itself
     */
    [[nodiscard]] std::shared_ptr<PolarTable<T>> squeeze() const;

    [[nodiscard]] std::shared_ptr<PolarTable<T>> resample(std::shared_ptr<DimensionGrid> new_dimension_grid) const;

   private:
    void reset();

    void build_interpolator();

   private:
    std::vector<T> m_values;

  };


  template<typename T>
  std::shared_ptr<PolarTable<T>> make_polar_table(const std::string &name,
                                                  const std::string &unit,
                                                  const std::string &description,
                                                  POEM_DATATYPE type,
                                                  const std::shared_ptr<DimensionGrid> &dimension_grid) {
    return std::make_shared<PolarTable<T>>(name, unit, description, type, dimension_grid);
  }

  inline std::shared_ptr<PolarTable<double>> make_polar_table_double(const std::string &name,
                                                                     const std::string &unit,
                                                                     const std::string &description,
                                                                     const std::shared_ptr<DimensionGrid> &dimension_grid) {
    return make_polar_table<double>(name, unit, description, POEM_DOUBLE, dimension_grid);
  }

  inline std::shared_ptr<PolarTable<int>> make_polar_table_int(const std::string &name,
                                                               const std::string &unit,
                                                               const std::string &description,
                                                               const std::shared_ptr<DimensionGrid> &dimension_grid) {
    return make_polar_table<int>(name, unit, description, POEM_INT, dimension_grid);
  }

} // namespace poem

#include "PolarTable.inl"

#endif //POEM_POLARTABLE_H
