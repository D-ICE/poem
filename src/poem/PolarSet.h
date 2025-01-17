//
// Created by frongere on 17/01/25.
//

#ifndef POEM_POLARSET_H
#define POEM_POLARSET_H

#include <string>
#include <memory>
#include <unordered_map>

#include "exceptions.h"
#include "DimensionGrid.h"
#include "Polar.h"

namespace poem {

  /**
   * A PolarSet stacks the different Polar for one OperationMode
   */
  class PolarSet {
   public:
    explicit PolarSet(const std::string &name) : m_name(name) {}

    std::shared_ptr<Polar> create_polar(POLAR_MODE mode,
                                       std::shared_ptr<DimensionGrid> dimension_grid) {

      std::string polar_name = m_name + "/" + polar_mode_to_string(mode);
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

}  // poem

#endif //POEM_POLARSET_H
