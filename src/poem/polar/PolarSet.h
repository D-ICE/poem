//
// Created by frongere on 13/04/23.
//

#ifndef POEM_POLARSET_H
#define POEM_POLARSET_H

#include <memory>

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
    using PolarMap = std::unordered_map<std::string, std::unique_ptr<PolarBase>>;
    using NameMap = std::unordered_map<std::string, std::string>;

    PolarSet(const Attributes &attributes, const Schema &schema, const Schema &newest_schema) :
        m_is_built(false),
        m_attributes(attributes),
        m_schema(schema),
        m_newest_schema(newest_schema) {

      if (!m_newest_schema.is_newest()) {
        spdlog::critical("Not the newest schema given to PolarSet");
      }


      // We automatically add the current schema as attribute if not found in the attributes
      if (!m_attributes.contains("schema")) {
        m_attributes.add_attribute("schema", schema.json_str());
      } else {
        // FIXME: Verify that this is the same attribute as given in the ctor!!
//        if (!m_attributes)
      }

      // Check that the given attributes are compliant with the current schema
      m_schema.check_attributes(&m_attributes);

      // Generating map
      generate_attributes_name_map();
    };

    template<typename T, size_t _dim>
    Polar<T, _dim> *New(const std::string &name,
                        const std::string &unit,
                        const std::string &description,
                        type::POEM_TYPES type,
                        std::shared_ptr<DimensionPointSet<_dim>> dimension_point_set) {

      if (!dimension_point_set->is_built()) dimension_point_set->build();

      auto polar = std::make_unique<Polar<T, _dim>>(name, unit, description, type, dimension_point_set);

      m_polars_map.insert({name, std::move(polar)});
      auto polar_ptr = static_cast<Polar<T, _dim> *>(m_polars_map[name].get());

      // Check that the polar is compliant with the current schema
      m_schema.check_polar<T, _dim>(polar_ptr);

      // Generate map
      generate_polar_name_map(name);

      return polar_ptr;
    }

    void build() {

      for (auto &polar : m_polars_map) {
        polar.second->build();
      }

      m_is_built = true;
    }


    bool is_using_newest_schema() const { return m_schema.is_newest(); }

    const Attributes &attributes() const { return m_attributes; }

    /*
     * TODO: ajouter tout ce qu'il faut pour acceder aux polaires, avec interpolation ND et mise en cache...
     */

    // TODO: voir si on garde
    template<typename T, size_t _dim>
    Polar<T, _dim> *get_polar(const std::string &name) const {
      std::string old_name;
      try {
        old_name = m_polar_name_map.at(name);
      } catch (const std::out_of_range &e) {
        spdlog::critical("Polar name {} does not exist.");
        CRITICAL_ERROR
      }

      return static_cast<Polar<T, _dim> *>(m_polars_map.at(old_name).get());
    }

    // TODO: voir si on garde
    template<typename T, size_t _dim>
    T eval(const std::string &name,
           const std::array<T, _dim> dimension_point) const { // TODO: point devrait etre type ???
      std::string old_name;
      try {
        old_name = m_polar_name_map.at(name);
      } catch (const std::out_of_range &e) {
        spdlog::critical(R"(Polar name "{}" does not exist in the newest schema. Please upgrade your code)", name);
        CRITICAL_ERROR
      }

      auto polar = m_polars_map.at(old_name).get();
      return *static_cast<double *>(polar->eval(&dimension_point));

    }


    int to_netcdf(const std::string &nc_file) const {

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
        for (const auto &attribute: m_attributes) {
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

    void generate_attributes_name_map() {
      // TODO
      for (const auto &attribute: m_attributes) {
        auto name = attribute.first;
        if (name == "schema") continue;

        if (m_schema.is_newest()) {
          // The attribute has not changed
          m_attributes_name_map.insert({name, name});

        } else {
          // The attribute may have changed
          auto current_name = m_newest_schema.get_newest_attribute_name(name);

          // Si current name est vide, on a pas trouve d'alias nulle part
          if (current_name.empty() && m_schema.get_attribute_schema(name).is_required()) {
            spdlog::critical("No schema compatibility for attribute named {}", name);
            CRITICAL_ERROR
          }

          m_attributes_name_map.insert({current_name, name});

        }

      }

    }

    void generate_polar_name_map(const std::string &polar_name) {

      if (m_schema.is_newest()) {
        m_polar_name_map.insert({polar_name, polar_name});

      } else {
        auto newest_name = m_newest_schema.get_newest_variable_name(polar_name);

        if (newest_name.empty() && m_schema.get_variable_schema(polar_name).is_required()) {
          spdlog::critical("No schema compatibility for polar named {}", polar_name);
          CRITICAL_ERROR
        }

        if (newest_name.empty()) {
          spdlog::critical("Variable name from old schema has no equivalent in the newest schema");
          CRITICAL_ERROR
          // TODO: voir i on ne fait qu'un warning ?
        }

        m_polar_name_map.insert({newest_name, polar_name});

      }

    }

   private:
    bool m_is_built;

    Attributes m_attributes;
    NameMap m_attributes_name_map;

    PolarMap m_polars_map;
    NameMap m_dimensions_name_map;
    NameMap m_polar_name_map;

    Schema m_schema;
    Schema m_newest_schema;
  };

}  // poem

#endif //POEM_POLARSET_H
