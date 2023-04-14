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

#include <netcdf>
#include <spdlog/spdlog.h>

#include "Dimensions.h"
#include "Variables.h"
#include "PolarPoint.h"


namespace poem {

  /**
   * Base class for a polar to be used for polymorphism into a PolarSet
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

    virtual void to_netcdf(netCDF::NcFile &dataFile) const = 0;

   protected:
    std::unique_ptr<VariableID> m_var_ID;

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
    using PolarPoints = std::map<const DimensionPoint<_dim> *, PolarPoint<T, _dim>>;

    Polar(const std::string &name,
          const std::string &unit,
          const std::string &description,
          type::POEM_TYPES type,
          std::shared_ptr<DimensionPointSet<_dim>> dimension_point_set) :
        PolarBase(name, unit, description, type),
        m_dimension_point_set(dimension_point_set) {


      for (const auto dimension_point: *m_dimension_point_set) {
        PolarPoint<T, _dim> polar_point(dimension_point);
        m_points.insert({dimension_point.get(), polar_point});
      }

    }

    const size_t dim() const override { return _dim; }

    void to_netcdf(netCDF::NcFile &dataFile) const override {

      // Storing variable
      if (!is_filled()) {
        spdlog::critical("Attempting to write a polar to disk while it is not totally populated");
        CRITICAL_ERROR
      }

      // Storing dimensions
      std::vector<netCDF::NcDim> dims;
      dims.reserve(_dim);

      for (size_t i=0; i<_dim; ++i) {
        auto dimension_ID = m_dimension_point_set->dimension_ID_set()->get(i);
        auto values = m_dimension_point_set->dimension_vector(i);

        std::string name(dimension_ID->name());

        // TODO: voir si on eut pas detecter que le nom est deja pris...
        // Declaration of a new dimension ID
        auto dim = dataFile.getDim(name);
        if (dim.isNull()) {
          dim = dataFile.addDim(name, values.size());

          // The dimension as a variable
          netCDF::NcVar nc_var = dataFile.addVar(name, netCDF::ncDouble, dim);
          nc_var.putVar(values.data());
          nc_var.putAtt("unit", dimension_ID->unit());
          nc_var.putAtt("description", dimension_ID->description());
          nc_var.putAtt("min", std::to_string(dimension_ID->min()));
          nc_var.putAtt("max", std::to_string(dimension_ID->max()));
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

        // Get the values as a flat vector
        std::vector<T> values;
        for (const auto &point : m_points) {
          values.push_back(point.second.value());
        }

        nc_var.putVar(values.data());
        nc_var.putAtt("unit", unit());
        nc_var.putAtt("description", description());

      } else {
        spdlog::critical("Attempting to store more than one time a variable with the same name");
        CRITICAL_ERROR
      }

    }

    void set_point(void *polar_point) override {

      auto polar_point_ = static_cast<PolarPoint<T, _dim> *>(polar_point);
      if (!polar_point_->has_value()) spdlog::critical("Attempting to set the polar with non-initialized polar point");

      const DimensionPoint<_dim> *dimension_point = polar_point_->dimension_point();

      auto &internal_polar_point = m_points.at(dimension_point);

      if (internal_polar_point.has_value()) {
        spdlog::warn("The same polar point has been set more than one time");
      }
      internal_polar_point.set_value(polar_point_->value());

    }

    bool is_filled() const {
      return std::all_of(m_points.begin(), m_points.end(),
                         [](std::pair<const DimensionPoint<_dim> *, PolarPoint<T, _dim>> x) {
                           return x.second.has_value();
                         }
      );
    }

    std::function<void(void *)> get_set_point_function() override {
      return [this](void *polar_point) {
        set_point(polar_point);
      };
    }

   private:
    std::shared_ptr<DimensionPointSet<_dim>> m_dimension_point_set;
    PolarPoints m_points;

  };

}  // poem

#endif //POEM_POLAR_H
