//
// Created by frongere on 08/04/23.
//

#ifndef POEM_POLAR_H
#define POEM_POLAR_H

#include <array>
#include <vector>
#include <memory>
#include <map>

#include "Attributes.h"
#include "Dimensions.h"
#include "Variables.h"


namespace poem {

  /*
   * Une polaire est une variable munie
   */

  class PolarBase {
   public:
    explicit PolarBase(const std::string &name,
                       const std::string &unit,
                       const std::string &description,
                       type::POEM_TYPES type) :
        m_var_ID(std::make_unique<VariableID>(name, unit, description, type)) {

    }

    const VariableID* variable_ID() const { return m_var_ID.get(); }


   protected:
    std::unique_ptr<VariableID> m_var_ID;

  };

  /**
   * Represents a point of a polar along with its dimension values
   * @tparam T type of the value
   * @tparam _dim dimension of the point
   */
  template<typename T, size_t _dim>
  class PolarPoint {
   public:
//    explicit PolarPoint(std::shared_ptr<DimensionPoint<_dim>> dimension_point) :
//        m_dimension_point(dimension_point),
//        m_value(0) {}

    PolarPoint(std::shared_ptr<DimensionPoint<_dim>> dimension_point, const T &val) :
        m_dimension_point(dimension_point),
        m_value(val) {}

   private:
    std::shared_ptr<DimensionPoint<_dim>> m_dimension_point;
    T m_value;
  };


  /**
   * Represents a polar for one variable
   * @tparam T
   * @tparam _dim
   */
  template<typename T, size_t _dim>
  class Polar : public PolarBase {
   public:
    Polar(const std::string &name,
          const std::string &unit,
          const std::string &description,
          type::POEM_TYPES type,
          std::shared_ptr<DimensionIDArray<_dim>> dimension_array) :
        PolarBase(name, unit, description, type),
        m_dim_ID_array(dimension_array) {}

    void push_bask(const PolarPoint<T, _dim> &polar_point) {
//      if (polar_point.m_dimension_point->m_ID_array != m_dim_ID_array.get()) {
//        STOP
//      }
      m_points.push_back(polar_point);
    }

//    PolarPoint<T, _dim> get_point() const {
//      return PolarPoint<T, _dim>();
//    }

//    const DimensionIDArray<_dim> *dimension_array() const {
//      return m_dim_ID_array.get();
//    }

   private:
    std::shared_ptr<DimensionIDArray<_dim>> m_dim_ID_array;
    std::vector<PolarPoint<T, _dim>> m_points;

  };

  class PolarSet {
   public:
    PolarSet() = default;

    template<typename T, size_t _dim>
    Polar<T, _dim> *New(const std::string &name,
                        const std::string &unit,
                        const std::string &description,
                        type::POEM_TYPES type,
                        std::shared_ptr<DimensionIDArray<_dim>> dim_ID_array) {
      auto polar = std::make_unique<Polar<T, _dim>>(name, unit, description, type, dim_ID_array);
      m_polars.push_back(std::move(polar));
      return static_cast<Polar<T, _dim> *>(m_polars.back().get());
    }


   private:
    std::vector<std::unique_ptr<PolarBase>> m_polars;
  };



//
//  enum POLAR_TYPE {
//    NDPOLAR_PPP,
//    NDPOLAR_VPP,
//    CDAPOLAR_PPP
//  };
//
//
//
////  enum ORDER {
////    ROW_MAJOR,
////    COLUMN_MAJOR
////  };
//
//  // FIXME: est-on certain de vouloir mettre version en template ???
//
//
//
//  class NDPolar {
//   public:
//    NDPolar(const std::string &name, const Attributes &attributes) :
//        m_name(name),
//        m_attributes(attributes) {};
//
//   private:
//    std::string m_name;
//
//    Attributes m_attributes;
//
//    std::map<std::string, std::unique_ptr<VariableBase>> m_variables;
//
//  };


}  // poem



#endif //POEM_POLAR_H
