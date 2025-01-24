//
// Created by frongere on 17/01/25.
//

#ifndef POEM_POLAR_H
#define POEM_POLAR_H

#include <string>
#include <memory>

#include "PolarNode.h"
#include "enums.h"

namespace poem {

  // Forward declaration
  class PolarSet;

  class PolarTableBase;

  template<typename T>
  class PolarTable;

  class DimensionGrid;

  /**
   * A Polar stacks the PolarTable for one POLAR_MODE
   *
   * Every PolarTable in a Polar share the same DimensionGrid
   */
  class Polar : public PolarNode {

   public:
    Polar(const std::string &name,
          POLAR_MODE mode,
          std::shared_ptr<DimensionGrid> dimension_grid);

    const POLAR_MODE &mode() const;

    std::shared_ptr<DimensionGrid> dimension_grid() const;

    std::shared_ptr<DimensionGrid> &dimension_grid();

    template<typename T>
    std::shared_ptr<PolarTable<T>> create_polar_table(const std::string &name,
                                                      const std::string &unit,
                                                      const std::string &description,
                                                      const POEM_DATATYPE type);

    void attach_polar_table(std::shared_ptr<PolarTableBase> polar_table);

    bool contains_polar_table(const std::string& name) const;

    std::shared_ptr<PolarTableBase> polar_table(const std::string &name) const;

    bool operator==(const Polar &other) const;

    bool operator!=(const Polar &other) const;

    std::shared_ptr<Polar> resample(std::shared_ptr<DimensionGrid> new_dimension_grid) const;

   private:
    POLAR_MODE m_mode;
    std::shared_ptr<DimensionGrid> m_dimension_grid;

  };

  template<typename T>
  std::shared_ptr<PolarTable<T>>
  Polar::create_polar_table(const std::string &name, const std::string &unit, const std::string &description,
                            const POEM_DATATYPE type) {

    auto polar_table = make_polar_table<T>(name, unit, description, type, m_dimension_grid);
    add_child(polar_table);
    return polar_table;
  }

  std::shared_ptr<Polar> make_polar(const std::string &name,
                                    POLAR_MODE mode,
                                    std::shared_ptr<DimensionGrid> dimension_grid);

}  // poem

#endif //POEM_POLAR_H
