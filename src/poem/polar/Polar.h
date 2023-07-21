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
#include "Variables.h"
#include "PolarPoint.h"


namespace poem {

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
    explicit PolarBase(const std::string &name,
                       const std::string &unit,
                       const std::string &description,
                       type::POEM_TYPES type) :
        Named(name, unit, description, type) {}

    virtual const size_t dim() const = 0;

    virtual size_t size() const = 0;

//    const std::string &name() const { return m_var_ID->name(); }
//
//    const std::string &unit() const { return m_var_ID->unit(); }
//
//    const std::string &description() const { return m_var_ID->description(); }
//
//    const type::POEM_TYPES &type() const { return m_var_ID->type(); }

    virtual void set_point(void *polar_point) = 0;

    virtual bool is_filled() const = 0;

    virtual void append(PolarBase *polar) = 0;

//    // FIXME: pas le plus elegant le void void...
//    virtual std::function<void(void *)> get_set_point_function() = 0;

    template<typename T, size_t _dim, typename = std::enable_if_t<std::is_same_v<T, double>>>
    double interp(const std::array<double, _dim> &dimension_point, bool bound_check) const {
      return static_cast<const InterpolablePolar<_dim> *>(this)->interp(dimension_point, bound_check);
    }

    template<typename T, size_t _dim>
    T nearest(const std::array<double, _dim> &dimension_point, bool bound_check) const {
      return static_cast<const Polar<T, _dim> *>(this)->nearest(dimension_point, bound_check);
    }

    virtual void to_netcdf(netCDF::NcFile &dataFile) const = 0;

   protected:
//    std::unique_ptr<VariableID> m_var_ID;

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

//    using PolarPoints = std::map<const DimensionPoint<_dim> *, PolarPoint<T, _dim>>;
//    using PolarPointsConstIter = typename PolarPoints::const_iterator;

    using InterpolatorND = mathutils::RegularGridInterpolator<double, _dim>;
    using NearestND = mathutils::RegularGridNearest<T, _dim, double>;

    Polar(const std::string &name,
          const std::string &unit,
          const std::string &description,
          type::POEM_TYPES type,
          std::shared_ptr<DimensionPointSet<_dim>> dimension_point_set) :
        PolarBase(name, unit, description, type),
        m_dimension_point_set(dimension_point_set),
        m_interpolator_is_built(false),
        m_nearest_is_built(false) {

      m_values.reserve(dimension_point_set->size());

//      for (const auto dimension_point: *m_dimension_point_set) {
//        PolarPoint<T, _dim> polar_point(dimension_point);
//        m_polar_points.insert({dimension_point.get(), polar_point});
//      }

    }

    const size_t dim() const override { return _dim; }

    size_t size() const override {
      NIY
//      return m_polar_points.size();
    }

//    std::shared_ptr<DimensionSet<_dim>> dimension_ID_set() {
//      return m_dimension_point_set->dimension_ID_set();
//    }

//    const DimensionPointSet<_dim> *dimension_point_set() const {
////      return m_dimension_point_set.get();
//    }

//    std::shared_ptr<DimensionPointSet<_dim>> dimension_point_set() {
//      return m_dimension_point_set;
//    }

    T nearest(const std::array<double, _dim> &dimension_point, bool bound_check) const {
      NIY
      if (!m_nearest_is_built) {
        const_cast<Polar<T, _dim> *>(this)->build_nearest();
      }

      // FIXME: bound_check non utilise encore dans RegularGridNearest
      return m_nearest->Nearest(dimension_point).val();
    }

    void to_netcdf(netCDF::NcFile &dataFile) const override {

      NIY
      // Storing variable
      if (!is_filled()) {
        // FIXME: is_filled est true meme si la variable est vide...
        spdlog::critical("Attempting to write a polar to disk while it is not totally populated");
        CRITICAL_ERROR
      }

//      if (!m_dimension_point_set->grid()) {
//        spdlog::critical(
//            "Unable to write NetCDF file as the polar's DimensionPointSet has no reference to a source grid");
//        CRITICAL_ERROR
//      }
//      auto grid = m_dimension_point_set->grid();
//
//      auto dimension_ID_set = m_dimension_point_set->dimension_ID_set();
//
//      // Storing dimensions
//      std::vector<netCDF::NcDim> dims;
//      dims.reserve(_dim);
//
//      for (size_t i = 0; i < _dim; ++i) {
//
//        auto dimension_ID = dimension_ID_set->get(i);
//        std::string name(dimension_ID->name());
//
//        auto values = grid->dimension_vector(i);
//
//        // TODO: voir si on eut pas detecter que le nom est deja pris...
//        // Declaration of a new dimension ID
//        auto dim = dataFile.getDim(name);
//        if (dim.isNull()) {
//          dim = dataFile.addDim(name, values.size());
//
//          // The dimension as a variable
//          netCDF::NcVar nc_var = dataFile.addVar(name, netCDF::ncDouble, dim);
//          nc_var.putVar(values.data());
//          /*
//           * FIXME: les attributs ici sont completement decorreles du schema...
//           *  il faudrait ajouter ces champs dynamiquement en amont et les stocker dans un vecteur
//           */
//
//          nc_var.putAtt("unit", dimension_ID->unit());
//          nc_var.putAtt("description", dimension_ID->description());
//          nc_var.putAtt("min", std::to_string(dimension_ID->min()));
//          nc_var.putAtt("max", std::to_string(dimension_ID->max()));
//        }
//
//        dims.push_back(dim);
//
//      }
//
//      // Storing the values
//      netCDF::NcVar nc_var = dataFile.getVar(name());
//
//      if (nc_var.isNull()) {
//
//        switch (type()) {
//          case type::DOUBLE:
//            nc_var = dataFile.addVar(name(), netCDF::ncDouble, dims);
//            break;
//          case type::INT:
//            nc_var = dataFile.addVar(name(), netCDF::ncInt, dims);
//
//        }
//        nc_var.setCompression(true, true, 5);
//
//        // Map the values in a flat vector
//        std::vector<T> values;
//        for (const auto &point: m_polar_points) {
//          values.push_back(point.second.value());
//        }
//
//        nc_var.putVar(values.data());
//        nc_var.putAtt("unit", unit());
//        nc_var.putAtt("description", description());
//
//      } else {
//        spdlog::critical("Attempting to store more than one time a variable with the same name");
//        CRITICAL_ERROR
//      }

    }

    void set_point(void *polar_point) override {
      NIY

//      auto polar_point_ = static_cast<PolarPoint<T, _dim> *>(polar_point);
//      if (!polar_point_->has_value()) {
//        spdlog::critical("Attempting to set the polar with non-initialized polar point");
//        CRITICAL_ERROR
//      }
//
//      const DimensionPoint<_dim> *dimension_point = polar_point_->dimension_point();
//
//      auto &internal_polar_point = m_polar_points.at(dimension_point);
//
//      if (internal_polar_point.has_value()) {
//        spdlog::warn("The same polar point has been set more than one time");
//      }
//      internal_polar_point.set_value(polar_point_->value());

    }

    bool is_filled() const override {
      NIY
//      return std::all_of(m_polar_points.begin(), m_polar_points.end(),
//                         [](std::pair<const DimensionPoint<_dim> *, PolarPoint<T, _dim>> x) {
//                           return x.second.has_value();
//                         }
//      );
    }

    void append(PolarBase *other) override {
      NIY

//      if (other->dim() != _dim) {
//        spdlog::critical("Attempting to append a polar of dimension {} to a polar of dimension {}", other->dim(), _dim);
//        CRITICAL_ERROR
//      }
//
//      if (other->type() != type()) {
//        spdlog::critical("Attempting to append a polars of different types");
//        CRITICAL_ERROR
//      }
//
//      auto other_ = static_cast<Polar<T, _dim> *>(other);
//
//      // Checking that we concatenate two polars with the same DimensionIDSet
//      if (*other_->dimension_ID_set() != *dimension_ID_set()) {
//        spdlog::critical("Attempting to append two polars with different coordinates");
//        CRITICAL_ERROR
//      }
//
//      // Polar has changed, interpolators cannot be initialized
//      m_nearest_is_built = false;
//      m_nearest = nullptr;
//
//      m_interpolator_is_built = false;
//      m_interpolator = nullptr;
//
//      // FIXME: il y a des check a faire sur la compatibilite des polaires qu'on concatene....
//      //  il faut append aussi les dimension_points
//      //  il faut invalider les interpolateurs egalement (nearest et interp si double)...
//      m_dimension_point_set->append(*other_->dimension_point_set());
//
//      auto ppiter = other_->begin();
//      for (; ppiter != other_->end(); ++ppiter) {
//        m_polar_points.insert(*ppiter);
//      }

    }

//    // FIXME: pas le plus elegant le void void...
//    std::function<void(void *)> get_set_point_function() override {
//      return [this](void *polar_point) {
//        set_point(polar_point);
//      };
//    }

//    PolarPointsConstIter begin() const {
//      return m_polar_points.cbegin();
//    }
//
//    PolarPointsConstIter end() const {
//      return m_polar_points.cend();
//    }

   private:

    void build_nearest() {
      NIY

//      if (!is_filled()) {
//        spdlog::critical("Attempting to build nearest table of a polar before it is totally filled");
//        CRITICAL_ERROR
//      }
//
//      m_nearest = std::make_unique<NearestND>();
//
//      using NDArray = boost::multi_array<T, _dim>;
//      using IndexArray = boost::array<typename NDArray::index, _dim>;
//      IndexArray shape;
//
//      size_t num_elements = 1;
//      for (size_t i = 0; i < _dim; ++i) {
//        auto dim_id = m_dimension_point_set->dimension_ID_set()->get(i);
//        auto dim_vector = m_dimension_point_set->dimension_vector(i);
//
//        shape[i] = dim_vector.size();
//        num_elements *= shape[i];
//
//        m_nearest->AddCoord(dim_vector);
//      }
//
//      std::vector<T> data;
//      data.reserve(num_elements);
//      for (const auto &point: m_polar_points) {
//        data.push_back(point.second.value());
//      }
//
//      NDArray array(shape);
//      std::copy(data.begin(), data.end(), array.data());
//      m_nearest->AddVar(array);
//
//      m_nearest_is_built = true;

    }

   protected:
    bool m_interpolator_is_built;
    bool m_nearest_is_built;

    std::shared_ptr<DimensionPointSet<_dim>> m_dimension_point_set;
    std::vector<T> m_values;
//    PolarPoints m_polar_points;

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
        CRITICAL_ERROR
      }

      this->m_interpolator = std::make_unique<typename Polar<double, _dim>::InterpolatorND>();

      using NDArray = boost::multi_array<double, _dim>;
      using IndexArray = boost::array<typename NDArray::index, _dim>;
      IndexArray shape;

      size_t num_elements = 1;
      for (size_t i = 0; i < _dim; ++i) {
        auto dim_id = this->m_dimension_point_set->dimension_ID_set()->get(i);
        auto dim_vector = this->m_dimension_point_set->dimension_vector(i);

        shape[i] = dim_vector.size();
        num_elements *= shape[i];

        this->m_interpolator->AddCoord(dim_vector);
      }

      std::vector<double> data;
      data.reserve(num_elements);
      for (const auto &point: this->m_polar_points) {
        data.push_back(point.second.value());
      }

      NDArray array(shape);
      std::copy(data.begin(), data.end(), array.data());
      this->m_interpolator->AddVar(array);

      this->m_interpolator_is_built = true;
    }

  };

}  // poem

#endif //POEM_POLAR_H
