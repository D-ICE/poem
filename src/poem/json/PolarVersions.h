//
// Created by frongere on 03/04/23.
//

#ifndef POEM_POLARVERSIONS_H
#define POEM_POLARVERSIONS_H

#include <filesystem>
#include <regex>
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <unordered_map>

#include <dunits/dunits.h>
#include "poem/exceptions.h"

namespace fs = std::filesystem;
using json = nlohmann::json;

/*
 * 1 - on fait l'utlitaire qui prend un fichier de polaire et qui alerte sur le fait que les champs ne sont pas raccord
 *     avec la version de la polaire
 *
 * Dans Esperado:
 *  il faut qu'apres l'ecriture, on teste chaque nom de variable pour voir si c'est consistant avec la version de la polaire
 *
 * Dans mship:
 *  * on doit avoir la retrocompatibilité, savoir lire une ancienne version -> un mapping
 *
 *  * mship doit toujours etre au niveau de la dernière version de polaire.
 *
 *
 * Différents utilitaires:
 *
 *  * prend une version et check si les champs sont raccord avec cette version
 *
 *  * prend une version de lecture (mship) et mappe vers les noms de champ d'une autre version inferieure (polaire)
 *
 *  *
 *
 *
 *
 */

namespace poem {

  class Variable {
   public:

    enum DIM_OR_VAR {
      DIMENSION,
      VARIABLE
    };

    Variable(DIM_OR_VAR dim_or_var,
             const std::string &name,
             const std::string &type,
             const std::string &description,
             const std::string &unit,
             const std::vector<std::string> &tags) :
        m_dim_or_var(dim_or_var),
        m_name(name),
        m_type(type),
        m_description(description),
        m_unit(unit),
        m_old_name(""),
        m_is_optional(true),
        m_is_new(false) {

      if (!dunits::UnitsChecker::getInstance().is_valid_unit(unit, true)) {
        throw std::runtime_error("Unit " + unit + " is not known by DUNITS lib");
      }

      for (const auto &tag: tags) {
        if (tag == "mandatory") {
          m_is_optional = false;

        } else if (tag == "new") {
          m_is_new = true;

        } else {
          throw std::runtime_error("Tag " + tag + " not known for variable " + name);

        }
      }


    }

    DIM_OR_VAR dim_or_var() const { return m_dim_or_var; }

    std::string name() const { return m_name; }

    std::string type() const { return m_type; }

    std::string description() const { return m_description; }

    std::string unit() const { return m_unit; }

    bool is_optional() const {
      return m_is_optional;
    }

    bool is_new() const {
      return m_is_new;
    }

    void set_old_name(const std::string &old_name) {
      assert(!old_name.empty());
      m_old_name = old_name;
    }

    bool has_changed() const {
      return !m_old_name.empty();
    }

   private:
    DIM_OR_VAR m_dim_or_var;
    std::string m_name;
    std::string m_type;
    std::string m_description;
    std::string m_unit;

    // Options
    std::string m_old_name;
    bool m_is_optional;
    bool m_is_new;


  };


  class PolarVersion {

   public:
    explicit PolarVersion(const fs::path &file, int version) : m_version(version) {

      if (!fs::exists(file)) {
        throw std::runtime_error(std::string(file) + " not found");
      }

      std::ifstream ifs(file);
      m_node = json::parse(ifs);

      // TODO: voir a extraire les coords et variables en speedup
      if (m_node["version"].get<int>() != version) {
        throw std::runtime_error("scheme version is not consistent");
      }

      auto variables_node = m_node["variables"];

      for (auto it = variables_node.begin(); it != variables_node.end(); ++it) {

        const std::string &name = it.key();
        auto type = (*it)["type_"].get<std::string>();
        auto description = (*it)["description"].get<std::string>();
        auto unit = (*it)["unit"].get<std::string>();

        std::vector<std::string> tags;
        if ((*it).find("tags") != (*it).end()) {
          tags = (*it)["tags"].get<std::vector<std::string>>();
        }

        auto var_entry = m_variables_map.insert({it.key(),
                                                 {Variable::DIM_OR_VAR::VARIABLE, name, type, description, unit,
                                                  tags}});
        if ((*it).find("old_name") != ((*it).end())) {
          (*var_entry.first).second.set_old_name((*it)["old_name"].get<std::string>());
        }

      }

      auto dimensions_node = m_node["dimensions"];

      for (auto it = dimensions_node.begin(); it != dimensions_node.end(); ++it) {

        const std::string &name(it.key());
        auto type = (*it)["type_"].get<std::string>();
        auto description = (*it)["description"].get<std::string>();
        auto unit = (*it)["unit"].get<std::string>();

        std::vector<std::string> tags;
        if ((*it).find("tags") != (*it).end()) {
          tags = (*it)["tags"].get<std::vector<std::string>>();
        }

        auto var_entry = m_dimensions_map.insert({it.key(),
                                                  {Variable::DIM_OR_VAR::DIMENSION, name, type, description, unit,
                                                   tags}});
        if ((*it).find("old_name") != ((*it).end())) {
          (*var_entry.first).second.set_old_name((*it)["old_name"].get<std::string>());
        }
      }


    }


    int version() const { return m_version; }

    std::string dump(int indent = 2) const {
      return m_node.dump(indent);
    }

    std::string find_corresponding_key(const std::string &name, Variable::DIM_OR_VAR dim_or_var) const {
      const std::unordered_map<std::string, Variable> *map =
          (dim_or_var == Variable::VARIABLE) ? &m_variables_map : &m_dimensions_map;

      for (auto it = map->begin(); it != map->end(); ++it) {
        if (std::regex_match(name, std::regex(it->first))) {
          return it->first;
        }
      }
      return "";
    }

    std::vector<std::string>
    unknown_names(const std::vector<std::string> &name_list,
                  Variable::DIM_OR_VAR dim_or_var) const {
      std::vector<std::string> unknown_names;
      for (const auto &name: name_list) {
        if (find_corresponding_key(name, dim_or_var).empty()) {
          unknown_names.push_back(name);
        }
      }
      return unknown_names;
    }

    Variable *get(const std::string &var_name, Variable::DIM_OR_VAR dim_or_var) {
      auto key = find_corresponding_key(var_name, dim_or_var);
      if (key.empty()) {
        return nullptr;
      } else {
        return &m_variables_map.at(key);
      }
    }

   private:
    int m_version;
    json m_node;

//    std::unordered_map<std::string, Dimension> m_dimensions_map;
    std::unordered_map<std::string, Variable> m_variables_map;
    std::unordered_map<std::string, Variable> m_dimensions_map;

  };

/**
 * Class that maps
 */
  class NameMapper {
   public:
    NameMapper(int version_from, int version_to, Variable::DIM_OR_VAR dim_or_var) :
        m_version_from(version_from),
        m_version_to(version_to),
        m_dim_or_var(dim_or_var) {

      assert(version_from >= version_to);


    }


   private:
    friend class PolarVersions;

    Variable::DIM_OR_VAR m_dim_or_var;
    int m_version_from;
    int m_version_to;

    std::unordered_map<std::string, std::string> m_mapper;
  };


  class PolarVersions {

   public:
    explicit PolarVersions(const fs::path &scheme_folder);

    int head_version() const { return m_head_version; }

    const PolarVersion &get(int version) const {
      check_version_bounds(version);
      return m_versions.at(version - 1);
    }

    std::vector<std::string>
    unknown_names(const std::vector<std::string> &name_list,
                  int version,
                  Variable::DIM_OR_VAR dim_or_var) const {
      return get(version).unknown_names(name_list, dim_or_var);
    }

    NameMapper get_mapper(int version_from, int version_to, Variable::DIM_OR_VAR dim_or_var) const {
      if (version_from < version_to) {
        throw std::runtime_error("version_from must be greater or equal to version_to");
      }
      NameMapper mapper(version_from, version_to, dim_or_var);
      /*
       * On itere sur le noms dans from et on cherche de proche en proche les noms de Vp correspondant en jouant sur les
       * tags
       */

      NIY
    }


   private:
    void check_version_bounds(int version) const {
      if (version < 1) {
        throw std::runtime_error("Polar version start at 1");
      }
      if (version > m_head_version) {
        throw std::runtime_error("Requested version higher than head");
      }
    }

   private:
    fs::path version_to_file(int version);

   private:
    fs::path m_scheme_folder;
    int m_head_version;
    std::vector<PolarVersion> m_versions;

  };

}  // poem

#endif //POEM_POLARVERSIONS_H
