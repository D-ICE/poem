//
// Created by frongere on 17/12/24.
//

#ifndef POEM_POLARTABLE_H
#define POEM_POLARTABLE_H

#include <string>

#include <boost/multi_array.hpp>
#include <MathUtils/RegularGridInterpolator.h>

#include "exceptions.h"
#include "Named.h"
#include "Dimension.h"
#include "DimensionSet.h"
#include "DimensionGrid.h"

namespace poem {

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
  // ===================================================================================================================

  // Forward declaration
  template<typename T>
  class PolarTable;

  /**
   * Non template base class for Interpolator class used by PolarTable
   */
  struct InterpolatorBase {
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
  // ===================================================================================================================

  /**
   * Non template base class for PolarTable
   */
  struct PolarTableBase {
    virtual POEM_DATATYPE type() const = 0;

    virtual const std::string &name() const = 0;

    virtual std::shared_ptr<DimensionGrid> dimension_grid() const = 0;
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

      switch (type) {
        case POEM_DOUBLE:
          if (!std::is_same_v<T, double>) {
            spdlog::critical("Type template argument and specified POEM_DATATYPE {} mismatch at the creation "
                             "of PolarTable {}", "POEM_DOUBLE", name);
            CRITICAL_ERROR_POEM
          }
          break;

        case POEM_INT:
          if (!std::is_same_v<T, int>) {
            spdlog::critical("Type template argument and specified POEM_DATATYPE {} mismatch at the creation "
                             "of PolarTable {}", "POEM_INT", name);
            CRITICAL_ERROR_POEM
          }
          break;
      }

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
    [[nodiscard]] std::shared_ptr<DimensionGrid> dimension_grid() const override {
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
     * Fill the table with the given value
     * @param value
     */
    void fill_with(T value) {
      m_values = std::vector<T>(dimension_grid()->size(), value);
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

      // FIXME: ce switch devrait plutot se trouver dans la class Interpolator !
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

      // FIXME: ce switch devrait plutot se trouver dans la classe Interpolator !
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

} // namespace poem

#endif //POEM_POLARTABLE_H
