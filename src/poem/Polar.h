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
#include <algorithm>
#include <type_traits>

#include <netcdf>
#include <spdlog/spdlog.h>
#include <MathUtils/RegularGridInterpolator.h>
#include <MathUtils/RegularGridNearest.h>

#include "Dimensions.h"

namespace poem {

  enum POLAR_TYPE {
    PPP,
    HPPP,
    HVPP_PB,
    HVPP_HP,
    VPP,
  };

  // forward declaration
  template<typename T, size_t _dim>
  class Polar;

  template<size_t _dim>
  class InterpolablePolar;

  /**
   * Base class for a polar to be used for polymorphism into a PolarSet
   */
  class PolarBase : public Named {
   public:
    PolarBase(const std::string &name,
              const std::string &unit,
              const std::string &description,
              type::POEM_TYPES type,
              POLAR_TYPE polar_type) :
        Named(name, unit, description, type),
        m_polar_type(polar_type) {}

    POLAR_TYPE polar_type() const { return m_polar_type; }

    virtual const size_t dim() const = 0;

    virtual size_t size() const = 0;

    virtual bool is_filled() const = 0;

//    virtual void append(PolarBase *polar) = 0;

    template<typename T, size_t _dim, typename = std::enable_if_t<std::is_same_v<T, double>>>
    double interp(const std::array<double, _dim> &dimension_point, bool bound_check) const {
      return static_cast<const InterpolablePolar<_dim> *>(this)->interp(dimension_point, bound_check);
    }

    template<typename T, size_t _dim>
    T nearest(const std::array<double, _dim> &dimension_point, bool bound_check) const {
      return static_cast<const Polar<T, _dim> *>(this)->nearest(dimension_point, bound_check);
    }

    virtual void to_netcdf(netCDF::NcGroup &dataFile) const = 0;

   protected:
    POLAR_TYPE m_polar_type;

  };

  /**
   * Represents a polar for one variable
   *
   * Should not be implemented directly but via a PolarSet
   *
   * @tparam T datatype of the polar
   * @tparam _dim number of dimensions of the polar
   */
  template<typename T, size_t _dim>
  class Polar : public PolarBase {

   public:

    using InterpolatorND = mathutils::RegularGridInterpolator<double, _dim>;
    using NearestND = mathutils::RegularGridNearest<T, _dim, double>;

    Polar(const std::string &name,
          const std::string &unit,
          const std::string &description,
          type::POEM_TYPES type,
          POLAR_TYPE polar_type,
          std::shared_ptr<DimensionPointSet<_dim>> dimension_point_set) :
        PolarBase(name, unit, description, type, polar_type),
        m_dimension_point_set(dimension_point_set),
        m_interpolator_is_built(false),
        m_nearest_is_built(false),
        m_values(std::vector<T>(dimension_point_set->size())) {}

    const size_t dim() const override { return _dim; }

    size_t size() const override {
      NIY_POEM
//      return m_polar_points.size();
    }

    void set_value(size_t idx, const T &value) {
      m_values.at(idx) = value;
    }

    void set_values(const std::vector<T> &values) {
      m_values = values;
    }

    T nearest(const std::array<double, _dim> &dimension_point, bool bound_check) const {

      if (!m_nearest_is_built) {
        const_cast<Polar<T, _dim> *>(this)->build_nearest();
      }

      // FIXME: bound_check non utilise encore dans RegularGridNearest
      return m_nearest->Nearest(dimension_point).val();
    }

    void to_netcdf(netCDF::NcGroup &dataFile) const override {

      // TODO: check qu'on va ecrire une polaire qui est bien remplie

      auto grid = m_dimension_point_set->dimension_grid(); // FIXME: dimension_grid est vide;..
      auto dimension_set = m_dimension_point_set->dimension_set();

      // Storing dimensions
      std::vector<netCDF::NcDim> dims;
      dims.reserve(_dim);

      for (size_t i = 0; i < _dim; ++i) {

        auto dimension = dimension_set->at(i);
        auto name = dimension->name();
        auto values = grid.values(i);

        // TODO: voir si on eut pas detecter que le nom est deja pris...
        // Declaration of a new dimension ID
        auto dim = dataFile.getDim(name);
        if (dim.isNull()) {
          dim = dataFile.addDim(name, values.size());

          // The dimension as a variable
          netCDF::NcVar nc_var = dataFile.addVar(name, netCDF::ncDouble, dim);
          nc_var.putVar(values.data());
          /*
           * FIXME: les attributs ici sont completement decorreles du schema...
           *  il faudrait ajouter ces champs dynamiquement en amont et les stocker dans un vecteur
           */

          nc_var.putAtt("unit", dimension->unit());
          nc_var.putAtt("description", dimension->description());
        }

        dims.push_back(dim);

      }

      // Storing the values
      netCDF::NcVar nc_var = dataFile.getVar(name());

      if (nc_var.isNull()) {

        switch (type()) {
          case type::DOUBLE:
            nc_var = dataFile.addVar(name(), netCDF::ncDouble, dims);
            break;
          case type::INT:
            nc_var = dataFile.addVar(name(), netCDF::ncInt, dims);

        }
        nc_var.setCompression(true, true, 5);

        nc_var.putVar(m_values.data());
        nc_var.putAtt("unit", unit());
        nc_var.putAtt("description", description());

      } else {
        spdlog::critical("Attempting to store more than one time a variable with the same name {}", name());
        CRITICAL_ERROR_POEM
      }

    }

    bool is_filled() const override {
      return m_values.size() == m_dimension_point_set->size();
    }

    void bounds(std::array<T, _dim> &min_bounds, std::array<T, _dim> &max_bounds) const {
      for (size_t idx = 0; idx < _dim; ++idx) {
        auto dim_values = m_dimension_point_set->dimension_grid().values(idx);
        min_bounds[idx] = dim_values[0];
        max_bounds[idx] = dim_values[dim_values.size() - 1];
      }
    }

    void bounds(const std::string &name, double &min, double &max) const {
      auto dim_values = m_dimension_point_set->dimension_grid().values(name);
      min = dim_values[0];
      max = dim_values[dim_values.size() - 1];
    }


   private:

    void build_nearest() {

      if (!is_filled()) {
        spdlog::critical("Attempting to build nearest table of a polar before it is totally filled");
        CRITICAL_ERROR_POEM
      }

      m_nearest = std::make_unique<NearestND>();

      using NDArray = boost::multi_array<T, _dim>;
      using IndexArray = boost::array<typename NDArray::index, _dim>;
      IndexArray shape;

      auto grid = m_dimension_point_set->dimension_grid();
      for (size_t idim = 0; idim < _dim; ++idim) {
        auto values = grid.values(idim);
        shape[idim] = values.size();
        m_nearest->AddCoord(values);
      }

      NDArray array(shape);
      std::copy(m_values.begin(), m_values.end(), array.data());
      m_nearest->AddVar(array);

      m_nearest_is_built = true;

    }

   protected:
    bool m_interpolator_is_built;
    bool m_nearest_is_built;

    std::shared_ptr<DimensionPointSet<_dim>> m_dimension_point_set;
    std::vector<T> m_values;

    std::unique_ptr<InterpolatorND> m_interpolator;
    std::unique_ptr<NearestND> m_nearest;

  };

  template<size_t _dim>
  class InterpolablePolar : public Polar<double, _dim> {

   public:
    double interp(const std::array<double, _dim> &dimension_point, bool bound_check) const {

      if (!this->m_interpolator_is_built) {
        const_cast<InterpolablePolar<_dim> *>(this)->build_interpolator();
      }

      return this->m_interpolator->Interp(dimension_point, bound_check);
    }

   private:
    void build_interpolator() {

      if (!this->is_filled()) {
        spdlog::critical("Attempting to build interpolator of a polar before it is totally filled");
        CRITICAL_ERROR_POEM
      }

      this->m_interpolator = std::make_unique<typename Polar<double, _dim>::InterpolatorND>();

      using NDArray = boost::multi_array<double, _dim>;
      using IndexArray = boost::array<typename NDArray::index, _dim>;
      IndexArray shape;

      auto grid = this->m_dimension_point_set->dimension_grid();
      for (size_t idim = 0; idim < _dim; ++idim) {
        auto values = grid.values(idim);
        shape[idim] = values.size();
        this->m_interpolator->AddCoord(values);
      }

      NDArray array(shape);
      std::copy(this->m_values.begin(), this->m_values.end(), array.data());
      this->m_interpolator->AddVar(array);

      this->m_interpolator_is_built = true;
    }

  };

}  // poem

#endif //POEM_POLAR_H
