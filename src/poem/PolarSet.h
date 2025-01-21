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
#include "enums.h"
#include "PolarNode.h"

namespace poem {

  // Forward declaration
  class Polar;

  /**
   * A PolarSet stacks the different Polar for one OperationMode
   */
  class PolarSet : public PolarNode {

   public:
    explicit PolarSet(const std::string &name);

    std::shared_ptr<Polar> create_polar(POLAR_MODE mode,
                                        std::shared_ptr<DimensionGrid> dimension_grid);

    void attach_polar(std::shared_ptr<Polar> polar);

    size_t nb_polars() const;

    bool has_polar(POLAR_MODE mode);

    std::shared_ptr<Polar> polar(POLAR_MODE mode);

    bool operator==(const PolarSet &other) const;

    bool operator!=(const PolarSet &other) const;

  };

  std::shared_ptr<PolarSet> make_polar_set(const std::string &name);

}  // poem

#endif //POEM_POLARSET_H
