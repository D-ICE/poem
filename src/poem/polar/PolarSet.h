//
// Created by frongere on 13/04/23.
//

#ifndef POEM_POLARSET_H
#define POEM_POLARSET_H

#include <memory>
#include <map>

#include "Polar.h"
#include "Attributes.h"

#include "poem/schema/Schema.h"

namespace poem {

  /**
   * Represents a set of polars.
   *
   * This is what we manipulate directly.
   */
  class PolarSet {
   public:
    using PolarMap = std::map<std::string, std::unique_ptr<PolarBase>>;
    using NameMap = std::map<std::string, std::string>;

    // FIXME: on ne doit plus fournir Attribute a PolarSet mais on le fournira au to_netcdf !!!
    PolarSet() {

//      if (!m_newest_schema.is_newest()) {
//        spdlog::warn("Not the newest schema given to PolarSet");
//      }


      // We automatically add the current schema as attribute if not found in the attributes
//      if (!m_attributes.contains("schema")) {
//        m_attributes.add_attribute("schema", schema.json_str());
//      } else {
//        // FIXME: Verify that this is the same attribute as given in the ctor!!
////        if (!m_attributes)
//      }

      // Check that the given attributes are compliant with the current schema
//      m_schema.check_attributes(&m_attributes);

      // Generating map
      // FIXME: ici on faisait les maps pour la back compatibility. Ou la faire maintenant qu'on met plus d'attributs
      // dans polarset ???
      // Ou alors c'est toujours PolarSet qui a les attributs mais on les ajoute quand on en a besoin seulement...
//      generate_attributes_name_map();
    };

    PolarSet(const PolarSet &other) :
//        m_attributes(other.m_attributes),
        m_attributes_name_map(other.m_attributes_name_map),
//        m_schema(other.m_schema),
//        m_newest_schema(other.m_newest_schema),
        m_polar_name_map(other.m_polar_name_map) {

      for (const auto &pair: other.m_polars_map) {
        copy_polar(pair.second.get());
      }

    }

    template<typename T, size_t _dim>
    Polar<T, _dim> *new_polar(const std::string &name,
                              const std::string &unit,
                              const std::string &description,
                              type::POEM_TYPES type,
                              POLAR_TYPE polar_type,
                              std::shared_ptr<DimensionPointSet<_dim>> dimension_point_set) {

      if (m_polars_map.find(name) != m_polars_map.end()) {
        spdlog::critical("Attempting to add polar with name {} twice", name);
        CRITICAL_ERROR_POEM
      }
//      if (!dimension_point_set->is_built()) dimension_point_set->build();

      auto polar = std::make_unique<Polar<T, _dim>>(name, unit, description, type, polar_type, dimension_point_set);

      m_polars_map.insert({name, std::move(polar)});
      auto polar_ptr = static_cast<Polar<T, _dim> *>(m_polars_map[name].get());

//      // Check that the polar is compliant with the current schema
//      m_schema.check_polar<T, _dim>(polar_ptr);

      // Generate map
      generate_polar_name_map(name);

      return polar_ptr;
    }

//    bool is_using_newest_schema() const { return m_schema.is_newest(); }

//    const Attributes &attributes() const { return m_attributes; }

    bool is_filled() const {
      bool is_filled = true;
      for (const auto &polar: m_polars_map) {
        if (!polar.second->is_filled()) {
          is_filled = false;
          break;
        }
      }

      return is_filled;
    }

    /*
     * TODO: ajouter tout ce qu'il faut pour acceder aux polaires, avec interpolation ND et mise en cache...
     */

    PolarBase *get_polar(const std::string &name) const {
      return m_polars_map.at(name).get();
    }

    template<typename T, size_t _dim, typename = std::enable_if_t<!std::is_same_v<T, double>>>
    Polar<T, _dim> *get_polar(const std::string &name) const {
//      std::string old_name;
//      try {
//        old_name = m_polar_name_map.at(name);
//      } catch (const std::out_of_range &e) {
//        spdlog::critical("Polar name {} does not exist.");
//        CRITICAL_ERROR_POEM
//      }
//
//      return static_cast<Polar<T, _dim> *>(m_polars_map.at(old_name).get());

      return static_cast<Polar<T, _dim> *>(m_polars_map.at(name).get());
    }

    template<typename T, size_t _dim, typename = std::enable_if_t<std::is_same_v<T, double>>>
    InterpolablePolar<_dim> *get_polar(const std::string &name) const {
//      std::string old_name;
//      try {
//        old_name = m_polar_name_map.at(name);
//      } catch (const std::out_of_range &e) {
//        spdlog::critical("Polar name {} does not exist.", name);
//        CRITICAL_ERROR_POEM
//      }
//
//      return static_cast<InterpolablePolar<_dim> *>(m_polars_map.at(old_name).get());

      return static_cast<InterpolablePolar<_dim> *>(m_polars_map.at(name).get());

    }

    std::vector<std::string> polar_names() const {
      std::vector<std::string> polar_names;
      for (const auto &polar: m_polars_map) {
        polar_names.push_back(polar.first);
      }
      return polar_names;
    }

//    void append(const PolarSet &other) {
//
//      // TODO: check que les attributs sont consistants entre this et other
//
//      if (m_polars_map.empty()) {
//        // Adding polars from other
//        for (const auto &pair: other.m_polars_map) {
//          auto polar = pair.second.get();
//          copy_polar(polar);
//        }
//
//      } else {
//        // Append other's polar data
//        for (const auto &pair: m_polars_map) {
//          auto polar = pair.second.get();
//          polar->append(other.get_polar(polar->name()));
//        }
//
//      }
//
//    }

    template<typename T, size_t _dim, typename = std::enable_if_t<std::is_same_v<T, double>>>
    T interp(const std::string &name,
             const std::array<double, _dim> dimension_point, // FIXME: pas T mais double pour les dimensions
             bool bound_check = true) const {

      std::string old_name;
      try {
        old_name = m_polar_name_map.at(name);
      } catch (const std::out_of_range &e) {
        spdlog::critical(R"(Polar name "{}" does not exist in the newest schema. Please upgrade your code)", name);
        CRITICAL_ERROR_POEM
      }

      auto polar = m_polars_map.at(old_name).get();
      return polar->interp<double, _dim>(dimension_point, bound_check);
    }

    template<typename T, size_t _dim>
    T nearest(const std::string &name,
              const std::array<double, _dim> dimension_point,
              bool bound_check = true) const {
      std::string old_name;
      try {
        old_name = m_polar_name_map.at(name);
      } catch (const std::out_of_range &e) {
        spdlog::critical(R"(Polar name "{}" does not exist in the newest schema. Please upgrade your code)", name);
        CRITICAL_ERROR_POEM
      }

      auto polar = m_polars_map.at(old_name).get();
      return polar->nearest<T, _dim>(dimension_point, bound_check);
    }

//    std::vector<PolarSet> split(const Splitter &splitter) const {
//
//      std::vector<PolarSet> polar_set_vector;
//      polar_set_vector.reserve(splitter.nchunks());
//
//
//
//      NIY
//
//    }

    int to_netcdf(const std::string &nc_file, const Attributes &attributes) const {

      fs::path nc_file_path(nc_file);
      if (nc_file_path.is_relative()) {
        nc_file_path = fs::current_path() / nc_file_path;
      }

      spdlog::info("Writing NetCDF file {}", std::string(nc_file_path));

      constexpr int nc_err = 2;

      try {

        // Create the file. The Replace parameter tells netCDF to overwrite
        // this file, if it already exists.
        netCDF::NcFile dataFile(std::string(nc_file_path), netCDF::NcFile::replace);

        // Writing attributes
        // TODO: les attributs seront fournis en argument de la methode

        NIY_POEM
        // FIXME: il faut ajouter le schema dans les attributs ici... ?
        for (const auto &attribute: attributes) {
          dataFile.putAtt(attribute.first, attribute.second);
        }

        for (const auto &polar: m_polars_map) {
          polar.second->to_netcdf(dataFile);
        }

      } catch (netCDF::exceptions::NcException &e) {
        std::cerr << e.what() << std::endl;
        return nc_err;
      }

      return 0;
    }

   private:

    void copy_polar(PolarBase *polar) {
      switch (polar->dim()) {
        case 1:
          return copy_polar < 1 > (polar);
        case 2:
          return copy_polar < 2 > (polar);
        case 3:
          return copy_polar < 3 > (polar);
        case 4:
          return copy_polar < 4 > (polar);
        case 5:
          return copy_polar < 5 > (polar);
        case 6:
          return copy_polar < 6 > (polar);
        default:
          spdlog::critical("Polar dimensions lower than 1 or higher than 6 are forbidden");
          CRITICAL_ERROR_POEM
      }
    }

    template<size_t _dim>
    void copy_polar(PolarBase *polar) {
      auto type = polar->type();
      switch (type) {
        case type::INT:
          copy_polar<int, _dim>(polar);
          break;
        case type::DOUBLE:
          copy_polar<double, _dim>(polar);
          break;
        default:
          spdlog::critical("Type is not managed yet");
          CRITICAL_ERROR_POEM
      }
    }

    template<typename T, size_t _dim>
    void copy_polar(PolarBase *polar) {
      NIY_POEM
//      auto polar_ = static_cast<Polar<T, _dim> *>(polar);
//
//      auto polar_copy = New<T, _dim>(polar_->name(),
//                                     polar_->unit(),
//                                     polar_->description(),
//                                     polar_->type(),
//                                     polar_->dimension_point_set());
//
//      // Filling with values
//      auto iter = polar_->begin();
//      for (; iter != polar_->end(); ++iter) {
//        PolarPoint<T, _dim> polar_point = (*iter).second;
//        polar_copy->set_point(&polar_point);
//      }
//
//      if (!polar_copy->is_filled()) { // Pourrait etre retire ?
//        spdlog::critical("Copying polar while not filled");
//        CRITICAL_ERROR
//      }

    }

    void generate_attributes_name_map() {
      NIY_POEM
      // TODO
//      for (const auto &attribute: m_attributes) {
//        auto name = attribute.first;
//        if (name == "schema") continue;
//
//        if (m_schema.is_newest()) {
//          // The attribute has not changed
//          m_attributes_name_map.insert({name, name});
//
//        } else {
//          // The attribute may have changed
//          auto current_name = m_newest_schema.get_newest_attribute_name(name);
//
//          // Si current name est vide, on a pas trouve d'alias nulle part
//          if (current_name.empty() && m_schema.get_attribute_schema(name).is_required()) {
//            spdlog::critical("No schema compatibility for attribute named {}", name);
//            CRITICAL_ERROR
//          }
//
//          m_attributes_name_map.insert({current_name, name});
//
//        }
//
//      }

    }

    void generate_polar_name_map(const std::string &polar_name) {

//      if (m_schema.is_newest()) {
//        m_polar_name_map.insert({polar_name, polar_name});
//
//      } else {
//        auto newest_name = m_newest_schema.get_newest_variable_name(polar_name);
//
//        if (newest_name.empty() && m_schema.get_variable_schema(polar_name).is_required()) {
//          spdlog::critical("No schema compatibility for polar named {}", polar_name);
//          CRITICAL_ERROR_POEM
//        }
//
//        if (newest_name.empty()) {
//          spdlog::critical("Variable name from old schema has no equivalent in the newest schema");
//          CRITICAL_ERROR_POEM
//          // TODO: voir i on ne fait qu'un warning ?
//        }
//
//        m_polar_name_map.insert({newest_name, polar_name});
//
//      }

    }

   protected:
    NameMap m_attributes_name_map;

    PolarMap m_polars_map;
//    NameMap m_dimensions_name_map;
    NameMap m_polar_name_map;

//    Schema m_schema;
//    Schema m_newest_schema;
  };

}  // poem

#endif //POEM_POLARSET_H
