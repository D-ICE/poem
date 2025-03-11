//
// Created by frongere on 18/02/25.
//

#include "PolarTable.h"

namespace poem {

  #ifdef POEM_JIT

  void PolarTableBase::jit_load() {
    jit::JITManager::getInstance().load_polar_table(as_polar_table());
  }

  void PolarTableBase::jit_unload() {
    jit::JITManager::getInstance().unload_polar_table(as_polar_table());
  }

  #endif //POEM_JIT

  template<>
  double PolarTable<double>::interp(const DimensionPoint &dimension_point, OUT_OF_BOUND_METHOD oob_method) const {

    // FIXME: si on a une table de int, on devrait reagir de maniere diffente et renvoyer un nearest
    //  remplacer bound_check par un enum ERROR, SATURATE, EXTRAPOLATE

//    if (m_type != POEM_DOUBLE) {
//      return nearest(dimension_point, oob_method);
//    }

    if (dimension_point.dimension_set() != m_dimension_grid->dimension_set()) {
      LogCriticalError("[PolarTable::interp] DimensionPoint has not the same DimensionSet as the PolarTable");
      CRITICAL_ERROR_POEM
    }

    if (!m_interpolator) {
      const_cast<PolarTable<double> *>(this)->build_interpolator();
    }

    // Out of bound management
    auto dimension_point_ = dimension_point; // TODO: voir si ca copie correctement
    size_t index = 0;
    for (auto &coord: dimension_point_) {
      if (coord < m_dimension_grid->min(index) || coord > m_dimension_grid->max(index)) {
        switch (oob_method) {
          case ERROR: {
            LogCriticalError("In PolarTable {}, while calling interp, out of bound value found for "
                             "dimension {}. Min: {}, Max: {}, Value: {}",
                             m_name, m_dimension_grid->dimension_set()->name(index),
                             m_dimension_grid->min(index), m_dimension_grid->max(index),
                             coord);
            LogCriticalError("Following your context, you may consider using SATURATE out of bound method "
                             "in interpolation");
            CRITICAL_ERROR_POEM
          }
          case SATURATE: {
            dimension_point_[index] = coord < m_dimension_grid->min(index) ?
                                      m_dimension_grid->min(index) : m_dimension_grid->max(index);
          }
            break;
          case EXTRAPOLATE: {
            NIY_POEM
          }
        }
      }
      index++;
    }


    // FIXME: ce switch devrait plutot se trouver dans la class Interpolator !
    double val;
    switch (dim()) {
      case 1:
        val = static_cast<Interpolator<double, 1> *>(m_interpolator.get())->interp(dimension_point_, false);
        break;
      case 2:
        val = static_cast<Interpolator<double, 2> *>(m_interpolator.get())->interp(dimension_point_, false);
        break;
      case 3:
        val = static_cast<Interpolator<double, 3> *>(m_interpolator.get())->interp(dimension_point_, false);
        break;
      case 4:
        val = static_cast<Interpolator<double, 4> *>(m_interpolator.get())->interp(dimension_point_, false);
        break;
      case 5:
        val = static_cast<Interpolator<double, 5> *>(m_interpolator.get())->interp(dimension_point_, false);
        break;
      case 6:
        val = static_cast<Interpolator<double, 6> *>(m_interpolator.get())->interp(dimension_point_, false);
        break;
      default:
        LogCriticalError("ND interpolation not supported for dimensions higher than 6 (found {})", dim());
        CRITICAL_ERROR_POEM
    }

    return val;
  }

  template<>
  int PolarTable<int>::interp(const poem::DimensionPoint &dimension_point,
                              poem::OUT_OF_BOUND_METHOD oob_method) const {
    return nearest(dimension_point, oob_method);
  }


}  // poem
