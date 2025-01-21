//
// Created by frongere on 21/01/25.
//

#include "PolarTable.h"
#include "Polar.h"
#include "DimensionGrid.h"

namespace poem {

  Polar::Polar(const std::string &name, POLAR_MODE mode, std::shared_ptr<DimensionGrid> dimension_grid) :
      PolarNode(name),
      m_mode(mode),
      m_dimension_grid(dimension_grid) {
    m_polar_node_type = POLAR;
  }

  const POLAR_MODE &Polar::mode() const { return m_mode; }

  std::shared_ptr<DimensionGrid> Polar::dimension_grid() const { return m_dimension_grid; }

  std::shared_ptr<DimensionGrid> &Polar::dimension_grid() { return m_dimension_grid; }

  void Polar::attach_polar_table(std::shared_ptr<PolarTableBase> polar_table) {
    if (m_dimension_grid != polar_table->dimension_grid()) {
      spdlog::critical("While adding PolarTable {} to Polar {}, DimensionGrid mismatch",
                       polar_table->name(), m_name);
      CRITICAL_ERROR_POEM
    }
    add_child(polar_table);
  }

  std::shared_ptr<PolarTableBase> Polar::polar_table(const std::string &name) const {
    return child<PolarTableBase>(name);
  }

  bool Polar::operator==(const Polar &other) const {
    bool equal = m_name == other.m_name;
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

  std::shared_ptr<Polar>
  make_polar(const std::string &name, POLAR_MODE mode, std::shared_ptr<DimensionGrid> dimension_grid) {
    return std::make_shared<Polar>(name, mode, dimension_grid);
  }

}  // poem
