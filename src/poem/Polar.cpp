//
// Created by frongere on 21/01/25.
//

#include "PolarTable.h"
#include "Polar.h"
#include "DimensionGrid.h"

namespace poem {

  Polar::Polar(const std::string &name, POLAR_MODE mode, std::shared_ptr<DimensionGrid> dimension_grid) :
      PolarNode(name, polar_mode_to_string(mode) + " polar"),
      m_mode(mode),
      m_dimension_grid(dimension_grid) {
    m_polar_node_type = POLAR;
  }

  const POLAR_MODE &Polar::mode() const { return m_mode; }

  std::shared_ptr<DimensionGrid> Polar::dimension_grid() const { return m_dimension_grid; }

  std::shared_ptr<DimensionGrid> &Polar::dimension_grid() { return m_dimension_grid; }

  void Polar::attach_polar_table(std::shared_ptr<PolarTableBase> polar_table) {
    if (m_dimension_grid != polar_table->dimension_grid()) {
      LogCriticalError("While adding PolarTable {} to Polar {}, DimensionGrid mismatch",
                       polar_table->name(), m_name);
      CRITICAL_ERROR_POEM
    }
    add_child(polar_table);
  }

  bool Polar::contains_polar_table(const std::string& name) const {
    return m_children.contains(name);
  }

  std::shared_ptr<PolarTableBase> Polar::polar_table(const std::string &name) const {
    return child<PolarTableBase>(name);
  }

  void Polar::remove_polar_table(const std::string &name) {
    remove_child(polar_table(name));
  }

  bool Polar::operator==(const Polar &other) const {
    bool equal = true;
    equal &= PolarNode::operator==(other);
    equal &= m_mode == other.m_mode;
    equal &= *m_dimension_grid == *other.m_dimension_grid;
    equal &= n_children() == other.n_children();

    for (const auto &polar_table: children<PolarTableBase>()) {
      equal &= *polar_table == *other.child<PolarTableBase>(polar_table->name());
    }
    return equal;
  }

  bool Polar::operator!=(const Polar &other) const {
    return !(other == *this);
  }

  std::shared_ptr<Polar> Polar::resample(std::shared_ptr<DimensionGrid> new_dimension_grid) const {
    auto new_polar = make_polar(m_name, m_mode, new_dimension_grid);
    for (const auto &polar_table: children<PolarTableBase>()) {
      switch (polar_table->type()) {
        case POEM_DOUBLE:
          new_polar->attach_polar_table(polar_table->as_polar_table_double()->resample(new_dimension_grid, EXTRAPOLATE));
          break;
        case POEM_INT:
          LogWarningError("In resampling of Polar {}, PolarTable of type POEM_INT is skipped", m_name);
//          new_polar->attach_polar_table(polar_table->as_polar_table_int()->resample(new_dimension_grid));
          break;
        default:
          LogCriticalError("Type not supported");
          CRITICAL_ERROR_POEM

      }
    }

    new_polar->attributes() = m_attributes;
    return new_polar;
  }

  std::shared_ptr<Polar>
  make_polar(const std::string &name, POLAR_MODE mode, std::shared_ptr<DimensionGrid> dimension_grid) {
    return std::make_shared<Polar>(name, mode, dimension_grid);
  }

}  // poem
