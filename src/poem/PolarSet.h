//
// Created by frongere on 13/04/23.
//

#ifndef POEM_POLARSET_H
#define POEM_POLARSET_H

#include <memory>
#include <map>
#include <thread>
#include <mutex>
#include <filesystem>

#include "Polar.h"
#include "Attributes.h"
#include "PolarType.h"

namespace fs = std::filesystem;

namespace poem {

  class PolarSet {
   public:
    using PolarMap = std::unordered_map<std::string, std::shared_ptr<PolarBase>>;

    explicit PolarSet(const Attributes &attributes) :
        m_attributes(attributes),
        m_polar_type(polar_type_s2enum(attributes["polar_type"])) {

      if (attributes["polar_type"] != "ND") {  // ND is for v0 that do not contain groups...
        if (!attributes.contains("name")) {
          spdlog::critical("name attribute is mandatory for PolarSet");
          CRITICAL_ERROR_POEM
        }
      }
    }

    const std::string &name() const { return m_attributes["name"]; }

    POLAR_TYPE polar_type() const {
      return polar_type_s2enum(m_attributes["polar_type"]);
    }

    const std::string &polar_type_str() const {
      return m_attributes["polar_type"];
    }

    template<typename T, size_t _dim>
    std::shared_ptr<Polar<T, _dim>> new_polar(const std::string &name,
                                              const std::string &unit,
                                              const std::string &description,
                                              type::POEM_TYPES type,
                                              std::shared_ptr<DimensionPointSet<_dim>> dimension_point_set) {

      // Thread safety
      std::lock_guard<std::mutex> lock(m_mutex);

      if (m_polars_map.find(name) != m_polars_map.end()) {
        return std::reinterpret_pointer_cast<Polar<T, _dim>>(m_polars_map.at(name));
      }

      auto polar = std::make_shared<Polar<T, _dim>>(name, unit, description, type, m_polar_type, dimension_point_set);

      m_polars_map.insert({name, polar});

      return polar;
    }

    std::shared_ptr<PolarBase> polar(const std::string &name) const {
      try {
        return m_polars_map.at(name);
      } catch (const std::out_of_range &e) {
        spdlog::critical("No polar with name {}", name);
        CRITICAL_ERROR_POEM
      }
    }

    template<typename T, size_t _dim, typename = std::enable_if_t<!std::is_same_v<T, double>>>
    std::shared_ptr<Polar<T, _dim>> polar(const std::string &name) const {
      return std::reinterpret_pointer_cast<Polar<T, _dim>>(polar(name));
    }

    template<typename T, size_t _dim, typename = std::enable_if_t<std::is_same_v<T, double>>>
    std::shared_ptr<InterpolablePolar<_dim>> polar(const std::string &name) const {
      return std::reinterpret_pointer_cast<InterpolablePolar<_dim>>(polar(name));
    }

    std::vector<std::string> polar_names() const {
      std::vector<std::string> polar_names;
      for (const auto &polar: m_polars_map) {
        polar_names.push_back(polar.first);
      }
      return polar_names;
    }

    std::mutex *mutex() {
      return &m_mutex;
    }


    int to_netcdf(netCDF::NcGroup &group) const {

      for (const auto &polar: m_polars_map) {
        polar.second->to_netcdf(group);
      }

      for (const auto &attribute: m_attributes) {
        group.putAtt(attribute.first, attribute.second);
      }

      return 0;
    }


    int to_netcdf(const std::string &nc_file) const {

      fs::path nc_file_path(nc_file);
      if (nc_file_path.is_relative()) {
        nc_file_path = fs::current_path() / nc_file_path;
      }

      spdlog::info("Writing NetCDF file {}", std::string(nc_file_path));

      constexpr int nc_err = 2;
      int code;

      try {

        netCDF::NcFile dataFile(std::string(nc_file_path), netCDF::NcFile::replace);
        code = to_netcdf(dataFile);

      } catch (netCDF::exceptions::NcException &e) {
        std::cerr << e.what() << std::endl;
        code = nc_err;
      }

      return code;
    }

//   private:
//
//    void copy_polar(std::shared_ptr<PolarBase> polar) {
//      switch (polar->dim()) {
//        case 1:
//          return copy_polar < 1 > (polar);
//        case 2:
//          return copy_polar < 2 > (polar);
//        case 3:
//          return copy_polar < 3 > (polar);
//        case 4:
//          return copy_polar < 4 > (polar);
//        case 5:
//          return copy_polar < 5 > (polar);
//        case 6:
//          return copy_polar < 6 > (polar);
//        default:
//          spdlog::critical("Polar dimensions lower than 1 or higher than 6 are forbidden");
//          CRITICAL_ERROR_POEM
//      }
//    }

//    template<size_t _dim>
//    void copy_polar(std::shared_ptr<PolarBase> polar) {
//      auto type = polar->type();
//      switch (type) {
//        case type::INT:
//          copy_polar<int, _dim>(polar);
//          break;
//        case type::DOUBLE:
//          copy_polar<double, _dim>(polar);
//          break;
//        default:
//          spdlog::critical("Type is not managed yet");
//          CRITICAL_ERROR_POEM
//      }
//    }

//    template<typename T, size_t _dim>
//    void copy_polar(std::shared_ptr<PolarBase> polar) {
//      NIY_POEM
////      auto polar_ = static_cast<Polar<T, _dim> *>(polar);
////
////      auto polar_copy = New<T, _dim>(polar_->name(),
////                                     polar_->unit(),
////                                     polar_->description(),
////                                     polar_->type(),
////                                     polar_->dimension_point_set());
////
////      // Filling with values
////      auto iter = polar_->begin();
////      for (; iter != polar_->end(); ++iter) {
////        PolarPoint<T, _dim> polar_point = (*iter).second;
////        polar_copy->set_point(&polar_point);
////      }
////
////      if (!polar_copy->is_filled()) { // Pourrait etre retire ?
////        spdlog::critical("Copying polar while not filled");
////        CRITICAL_ERROR
////      }
//
//    }

//    void generate_attributes_name_map() {
//      NIY_POEM
//      // TODO
////      for (const auto &attribute: m_attributes) {
////        auto name = attribute.first;
////        if (name == "schema") continue;
////
////        if (m_schema.is_newest()) {
////          // The attribute has not changed
////          m_attributes_name_map.insert({name, name});
////
////        } else {
////          // The attribute may have changed
////          auto current_name = m_newest_schema.get_newest_attribute_name(name);
////
////          // Si current name est vide, on a pas trouve d'alias nulle part
////          if (current_name.empty() && m_schema.get_attribute_schema(name).is_required()) {
////            spdlog::critical("No schema compatibility for attribute named {}", name);
////            CRITICAL_ERROR
////          }
////
////          m_attributes_name_map.insert({current_name, name});
////
////        }
////
////      }
//
//    }

//    void generate_polar_name_map(const std::string &polar_name) {
////      NIY_POEM
//
////      if (m_schema.is_newest()) {
////        m_polar_name_map.insert({polar_name, polar_name});
////
////      } else {
////        auto newest_name = m_newest_schema.get_newest_variable_name(polar_name);
////
////        if (newest_name.empty() && m_schema.get_variable_schema(polar_name).is_required()) {
////          spdlog::critical("No schema compatibility for polar named {}", polar_name);
////          CRITICAL_ERROR_POEM
////        }
////
////        if (newest_name.empty()) {
////          spdlog::critical("Variable name from old schema has no equivalent in the newest schema");
////          CRITICAL_ERROR_POEM
////          // TODO: voir i on ne fait qu'un warning ?
////        }
////
////        m_polar_name_map.insert({newest_name, polar_name});
////
////      }
//
//    }

   protected:
    Attributes m_attributes;
    POLAR_TYPE m_polar_type;

    PolarMap m_polars_map;

    static inline std::mutex m_mutex;
  };

}  // poem

#endif //POEM_POLARSET_H
