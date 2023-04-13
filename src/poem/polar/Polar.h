//
// Created by frongere on 08/04/23.
//

#ifndef POEM_POLAR_H
#define POEM_POLAR_H

#include <array>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <iostream>

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

//    const VariableID *variable_ID() const { return m_var_ID.get(); }

    virtual const size_t dim() const = 0;

    const std::string &name() const { return m_var_ID->name(); }

    const std::string &unit() const { return m_var_ID->unit(); }

    const std::string &description() const { return m_var_ID->description(); }

    const type::POEM_TYPES &type() const { return m_var_ID->type(); }

    virtual void set_point(void *polar_point) = 0;

    virtual std::function<void(void *)> get_set_point_function() = 0;

    virtual void to_netcdf() const = 0;

//    virtual void* begin() const = 0;
//    virtual void* end() const = 0;

   protected:
    std::unique_ptr<VariableID> m_var_ID;
    size_t m_dim;

  };

  /**
   * Represents a point of a polar along with its dimension values
   * @tparam T type of the value
   * @tparam _dim dimension of the point
   */
  template<typename T, size_t _dim>
  class PolarPoint {
   public:
    PolarPoint(std::shared_ptr<DimensionPoint<_dim>> dimension_point, const T &val) :
        m_dimension_point(dimension_point),
        m_value(val) {}

    const T &value() const { return m_value; }

    const DimensionPoint<_dim>* dimension_point() const { return m_dimension_point.get(); }

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
    using PolarPoints = std::map<const DimensionPoint<_dim>*, PolarPoint<T, _dim>>;
//    using Iter = typename PolarPoints::const_iterator;

    Polar(const std::string &name,
          const std::string &unit,
          const std::string &description,
          type::POEM_TYPES type,
          std::shared_ptr<DimensionPointSet<_dim>> dimension_point_set) :
        PolarBase(name, unit, description, type),
        m_is_filled(false),
        m_dimension_point_set(dimension_point_set) {


      for (const auto dimension_point : *m_dimension_point_set) {
        PolarPoint<T, _dim> polar_point(dimension_point, 0.);
        m_points.insert({dimension_point.get(), polar_point});
      }

    }

    const size_t dim() const override { return _dim; }

    void to_netcdf() const override {
      NIY
    }

    void set_point(void *polar_point) override {

      auto polar_point_ = static_cast<PolarPoint<T, _dim> *>(polar_point);

      const DimensionPoint<_dim>* dimension_point = polar_point_->dimension_point();

      m_points.at(dimension_point) = *polar_point_;
      /*
       * TODO:
       * 1- verifier que le dimension point correspond bien au prochain push_back de valeur
       * 2- push du polar point
       * 3- marque comme filled si on a tout
       */

      std::cout << polar_point_->value() << std::endl;
    }


    std::function<void(void *)> get_set_point_function() override {
      return [this](void *polar_point) {
        set_point(polar_point);
      };
    }

   private:
    bool m_is_filled;

    std::shared_ptr<DimensionPointSet<_dim>> m_dimension_point_set;
    PolarPoints m_points;

  };

  class PolarSet {
   public:
    using PolarVector = std::vector<std::unique_ptr<PolarBase>>;
    using Iter = PolarVector::const_iterator;

    PolarSet() = default;

    template<typename T, size_t _dim>
    Polar<T, _dim> *New(const std::string &name,
                        const std::string &unit,
                        const std::string &description,
                        type::POEM_TYPES type,
                        std::shared_ptr<DimensionPointSet<_dim>> dimension_point_set) {

      auto polar = std::make_unique<Polar<T, _dim>>(name, unit, description, type, dimension_point_set);
      m_polars.push_back(std::move(polar));
      return static_cast<Polar<T, _dim> *>(m_polars.back().get());
    }

    Iter begin() const {
      return m_polars.cbegin();
    }

    Iter end() const {
      return m_polars.cend();
    }

   private:
    PolarVector m_polars;
  };

}  // poem



#endif //POEM_POLAR_H
