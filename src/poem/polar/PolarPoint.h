//
// Created by frongere on 13/04/23.
//

#ifndef POEM_POLARPOINT_H
#define POEM_POLARPOINT_H

#include <memory>
#include "Dimensions.h"

namespace poem {

  /**
 * Represents a point of a polar along with its dimension values
 * @tparam T type of the value
 * @tparam _dim dimension of the point
 */
  template<typename T, size_t _dim>
  class PolarPoint {
   public:
    PolarPoint(std::shared_ptr<DimensionPoint<_dim>> dimension_point) :
        m_dimension_point(dimension_point),
        m_has_value(false) {}

    PolarPoint(std::shared_ptr<DimensionPoint<_dim>> dimension_point, const T &val) :
        m_dimension_point(dimension_point),
        m_value(val),
        m_has_value(true) {}

    void set_value(const T &val) {
      m_value = val;
      m_has_value = true;
    }

    const T &value() const {
      return m_value;
    }

    bool has_value() const { return m_has_value; }

    const DimensionPoint<_dim> *dimension_point() const { return m_dimension_point.get(); }

   private:
    bool m_has_value;
    std::shared_ptr<DimensionPoint<_dim>> m_dimension_point;
    T m_value;
  };

}  // poem

#endif //POEM_POLARPOINT_H
