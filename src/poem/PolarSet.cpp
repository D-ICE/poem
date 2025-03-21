//
// Created by frongere on 21/01/25.
//

#include "Polar.h"
#include "PolarSet.h"

namespace poem {

  PolarSet::PolarSet(const std::string &name, const std::string& description) :
      PolarNode(name, description) {
    m_polar_node_type = POLAR_SET;
  }

  std::shared_ptr<Polar> PolarSet::create_polar(POLAR_MODE mode, std::shared_ptr<DimensionGrid> dimension_grid) {

    // FIXME: voir si la construction auto du nom est une bonne chose
//      std::string polar_name = m_name + "/" + polar_mode_to_string(mode);
    std::string polar_name = polar_mode_to_string(mode);
    if (has_polar(mode)) {
      LogWarningError("In PolarSet {}, Polar {} already exists", m_name, polar_name);
      return child<Polar>(polar_mode_to_string(mode));
    }

    auto polar = std::make_shared<Polar>(polar_name, mode, dimension_grid);
    add_child(polar);
    return polar;
  }

  void PolarSet::attach_polar(std::shared_ptr<Polar> polar) {
    if (has_polar(polar->mode())) {
      LogCriticalError("In PolarSet {}, Polar with mode {} was already existing",
                       m_name, polar_mode_to_string(polar->mode()));
      CRITICAL_ERROR_POEM
    }

    add_child(polar);
  }

  size_t PolarSet::nb_polars() const {
    return n_children();
  }

  bool PolarSet::has_polar(POLAR_MODE mode) {
    return m_children.contains(polar_mode_to_string(mode));
  }

  std::shared_ptr<Polar> PolarSet::polar(POLAR_MODE mode) {
    if (!has_polar(mode)) {
      LogCriticalError("PolarSet {} has no Polar of type {}", m_name, polar_mode_to_string(mode));
      CRITICAL_ERROR_POEM
    }
    return child<Polar>(polar_mode_to_string(mode));
  }

  bool PolarSet::operator==(const PolarSet &other) const {
    // We do not compare name, just enclosed polars
    bool equal = true;
    equal &= PolarNode::operator==(other);
    equal &= nb_polars() == other.nb_polars();
    for (const auto &polar: children<Polar>()) {
      equal &= *polar == *child<Polar>(polar->name());
    }
    return equal;
  }

  bool PolarSet::operator!=(const PolarSet &other) const {
    return !(other == *this);
  }

  std::shared_ptr<PolarSet> make_polar_set(const std::string &name, const std::string& description) {
    return std::make_shared<PolarSet>(name, description);
  }

}  // poem
