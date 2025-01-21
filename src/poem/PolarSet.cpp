//
// Created by frongere on 21/01/25.
//

#include "PolarSet.h"
#include "OperationMode.h"

namespace poem {

  const std::string& PolarSet::name() const {
    return m_name;
  }

  std::string PolarSet::full_name() const {
    std::string full_name_;
    if (m_operation_mode_parent) {
      full_name_ = m_operation_mode_parent->full_name().string();
    } else {
      full_name_ = "/" + m_name;
    }
    return full_name_;
  }

  std::shared_ptr<Polar> PolarSet::create_polar(POLAR_MODE mode, std::shared_ptr<DimensionGrid> dimension_grid) {

    // FIXME: voir si la construction auto du nom est une bonne chose
//      std::string polar_name = m_name + "/" + polar_mode_to_string(mode);
    std::string polar_name = polar_mode_to_string(mode);
    if (has_polar(mode)) {
      spdlog::warn("In PolarSet {}, Polar {} already exists", m_name, polar_name);
      return m_polars[mode];
    }

    auto polar = std::make_shared<Polar>(polar_name, mode, dimension_grid);
    polar->set_polar_set_parent(this);

    m_polars.insert({mode, polar});
    return polar;
  }

  void PolarSet::add_polar(std::shared_ptr<Polar> polar) {
    if (has_polar(polar->mode())) {
      spdlog::critical("In PolarSet {}, Polar with mode {} was already existing",
                       m_name, polar_mode_to_string(polar->mode()));
      CRITICAL_ERROR_POEM
    }

    polar->set_polar_set_parent(this);

    m_polars.insert({polar->mode(), polar});
  }

  size_t PolarSet::nb_polars() const {
    return m_polars.size();
  }

  bool PolarSet::has_polar(POLAR_MODE mode) { return m_polars.contains(mode); }

  std::shared_ptr<Polar> PolarSet::polar(POLAR_MODE mode) {
    if (!has_polar(mode)) {
      spdlog::critical("PolarSet {} has no Polar of type {}", m_name, polar_mode_to_string(mode));
      CRITICAL_ERROR_POEM
    }
    return m_polars[mode];
  }

  bool PolarSet::operator==(const PolarSet &other) const {
    // We do not compare name, just enclosed polars
    // TODO: voir si on doit quand meme comparer les noms...
    bool equal = nb_polars() == other.nb_polars();
    for (const auto &polar : m_polars) {
      equal &= *polar.second == *m_polars.at(polar.first);
    }
    return equal;
  }

  bool PolarSet::operator!=(const PolarSet &other) const {
    return !(other == *this);
  }

  PolarSet::PolarMapIter PolarSet::begin() {
    return m_polars.begin();
  }

  PolarSet::PolarMapIter PolarSet::end() {
    return m_polars.end();
  }

  std::shared_ptr<PolarSet> make_polar_set(const std::string &name) {
    return std::make_shared<PolarSet>(name);
  }
}  // poem
