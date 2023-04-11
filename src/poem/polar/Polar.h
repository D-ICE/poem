//
// Created by frongere on 08/04/23.
//

#ifndef POEM_POLAR_H
#define POEM_POLAR_H

#include <array>
#include <vector>
#include <memory>
#include <map>

namespace poem {

  enum POLAR_TYPE {
    NDPOLAR_PPP,
    NDPOLAR_VPP,
    CDAPOLAR_PPP
  };



//  enum ORDER {
//    ROW_MAJOR,
//    COLUMN_MAJOR
//  };

  // FIXME: est-on certain de vouloir mettre version en template ???



//  template <size_t dim_, typename VarType_>
//  class VarPolar {
//   public:
//    explicit VarPolar(std::shared_ptr<Dimensions<dim_>> dimensions) : m_dimensions(dimensions) {}
//
//   protected:
//    std::shared_ptr<Dimensions<dim_>> m_dimensions;
//    std::vector<VarType_> m_var;  // Linear representation
//
//  };
//
//  template <int version, size_t dim_, typename VarType_>
//  class VarPolatInterp : public VarPolar<dim_, VarType_> {
//   public:
//    VarPolatInterp() = default;
//
//   protected:
//
//
//  };

}  // poem



#endif //POEM_POLAR_H
