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

  // Forward declaration
  class OperationMode;

  /**
   * A PolarSet stacks the different Polar for one OperationMode
   */
 class PolarSet : public std::enable_shared_from_this<PolarSet> {
   public:
    using PolarMap = std::unordered_map<POLAR_MODE, std::shared_ptr<Polar>>;
    using PolarMapIter = PolarMap::iterator;

   public:
    explicit PolarSet(const std::string &name) : m_name(name) {}

    std::string full_name() const;

    std::shared_ptr<Polar> create_polar(POLAR_MODE mode,
                                        std::shared_ptr<DimensionGrid> dimension_grid);

    void add_polar(std::shared_ptr<Polar> polar);

    size_t nb_polars() const;

    bool has_polar(POLAR_MODE mode);

    std::shared_ptr<Polar> polar(POLAR_MODE mode);

    bool operator==(const PolarSet &other) const;

    bool operator!=(const PolarSet &other) const;

    PolarMapIter begin();

    PolarMapIter end();

   private:
    std::string m_name;
    PolarMap m_polars;

   std::shared_ptr<OperationMode> m_operation_mode_parent;

  };

  std::shared_ptr<PolarSet> make_polar_set(const std::string &name);

}  // poem

#endif //POEM_POLARSET_H
