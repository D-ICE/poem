//
// Created by frongere on 21/01/25.
//

//#include "PolarTable.h"
#include "Polar.h"

namespace poem {

  template<typename T>
  PolarTable<T>::PolarTable(const std::string &name, const std::string &unit, const std::string &description,
                            POEM_DATATYPE type, std::shared_ptr<DimensionGrid> dimension_grid) :
      Named(name, unit, description),
      m_type(type),
      m_dimension_grid(dimension_grid),
      m_values(dimension_grid->size()),
      m_interpolator(nullptr),
      m_polar_parent(nullptr) {

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

  template<typename T>
  const std::string &PolarTable<T>::name() const { return Named::m_name; }

  template<typename T>
  std::string PolarTable<T>::full_name() const {
    std::string full_name_;
    if (m_polar_parent) {
      full_name_ = m_polar_parent->full_name() + "/" + m_name;
    } else {
      full_name_ = "/" + m_name;
    }
    return full_name_;
  }

  template<typename T>
  POEM_DATATYPE PolarTable<T>::type() const { return m_type; }

  template<typename T>
  size_t PolarTable<T>::size() const {
    return m_dimension_grid->size();
  }

  template<typename T>
  size_t PolarTable<T>::size(size_t idx) const {
    return m_dimension_grid->size(idx);
  }

  template<typename T>
  size_t PolarTable<T>::dim() const {
    return m_dimension_grid->dimension_set()->size();
  }

  template<typename T>
  std::vector<size_t> PolarTable<T>::shape() const {
    return m_dimension_grid->shape();
  }

  template<typename T>
  std::shared_ptr<DimensionGrid> PolarTable<T>::dimension_grid() const {
    return m_dimension_grid;
  }

  template<typename T>
  const std::vector<DimensionPoint> &PolarTable<T>::dimension_points() const {
    return m_dimension_grid->dimension_points();
  }

  template<typename T>
  void PolarTable<T>::set_value(size_t idx, const T &value) {
    m_values[idx] = value;
    reset();
  }

  template<typename T>
  const vector<T> &PolarTable<T>::values() const {
    return m_values;
  }

  template<typename T>
  vector<T> &PolarTable<T>::values() {
    return m_values;
  }

  template<typename T>
  void PolarTable<T>::set_values(const vector<T> &new_values) {
    if (new_values.size() != m_values.size()) {
      spdlog::critical("Attempting to set values in PolarTable of different size ({} and {})",
                       m_values.size(), new_values.size());
      CRITICAL_ERROR_POEM
    }
    m_values = new_values;
  }

  template<typename T>
  void PolarTable<T>::fill_with(T value) {
    m_values = std::vector<T>(dimension_grid()->size(), value);
  }

  template<typename T>
  std::shared_ptr<PolarTable<T>> PolarTable<T>::copy() const {
    auto polar_table = std::make_shared<PolarTable<T>>(m_name, m_unit, m_description, m_type, m_dimension_grid);
    polar_table->set_values(m_values);
    return polar_table;
  }

  template<typename T>
  void PolarTable<T>::multiply_by(const T &coeff) {
    for (auto &val: m_values) {
      val *= coeff;
    }
  }

  template<typename T>
  void PolarTable<T>::offset(const T &val) {
    for (auto &val_: m_values) {
      val_ += val;
    }
  }

  template<typename T>
  void PolarTable<T>::sum(std::shared_ptr<PolarTable<T>> other) {
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

  template<typename T>
  void PolarTable<T>::abs() {
    for (auto &val: m_values) {
      val = std::abs(val);
    }
  }

  template<typename T>
  T PolarTable<T>::mean() const {
    T mean = 0;
    for (const auto &val: m_values) {
      mean += val;
    }
    return mean / (T) size();
  }

  template<typename T>
  bool PolarTable<T>::operator==(const PolarTableBase &other) const {
    if (m_type != other.type()) return false;
    auto other_ = static_cast<const PolarTable<T> *>(&other);
    bool equal = *m_dimension_grid == *other_->m_dimension_grid;
    equal &= m_values == other_->m_values;
    return equal;
  }

  template<typename T>
  bool PolarTable<T>::operator!=(const PolarTableBase &other) const {
    return !(other == *this);
  }

  template<typename T>
  T PolarTable<T>::nearest(const DimensionPoint &dimension_point) const {

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

  template<typename T>
  T PolarTable<T>::interp(const DimensionPoint &dimension_point, bool bound_check) const {
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

  template<typename T>
  std::shared_ptr<PolarTable<T>> PolarTable<T>::slice(std::unordered_map<std::string, double> prescribed_values) const {

    // Check that names are existing
    auto dimension_set = m_dimension_grid->dimension_set();
    for (const auto &pair: prescribed_values) {
      if (!dimension_set->contains(pair.first)) {
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

  template<typename T>
  bool PolarTable<T>::squeeze() {

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

  template<typename T>
  std::shared_ptr<PolarTable<T>> PolarTable<T>::squeeze() const {
    std::shared_ptr<PolarTable<T>> polar_table;

    auto polar_table_ = copy();
    if (polar_table_->squeeze()) {
      polar_table = polar_table_;
    } else {
      polar_table = this->shared_from_this();
    }
    return polar_table;
  }

  template<typename T>
  std::shared_ptr<PolarTable<T>> PolarTable<T>::resample(std::shared_ptr<DimensionGrid> new_dimension_grid) const {

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

  template<typename T>
  void PolarTable<T>::set_polar_parent(Polar *polar) {
    m_polar_parent = polar;
  }

  template<typename T>
  void PolarTable<T>::reset() {
    m_interpolator.reset();
  }

  template<typename T>
  void PolarTable<T>::build_interpolator() {

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

}  // poem
