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
             const std::vector<std::string> &options) :
        m_dim_or_var(dim_or_var),
        m_name(name),
        m_type(type),
        m_description(description),
        m_unit(unit) {

      if (!dunits::UnitsChecker::getInstance().is_valid_unit(unit, true)) {
        throw std::runtime_error("Unit " + unit + " is not known by DUNITS lib");
      }

    }

    DIM_OR_VAR dim_or_var() const { return m_dim_or_var; }

    std::string name() const { return m_name; }

    std::string type() const { return m_type; }

    std::string description() const { return m_description; }

    std::string unit() const { return m_unit; }

   private:
    DIM_OR_VAR m_dim_or_var;
    std::string m_name;
    std::string m_type;
    std::string m_description;
    std::string m_unit;

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

        std::vector<std::string> options;
        if ((*it).find("options") != (*it).end()) {
          options = (*it).get<std::vector<std::string>>();
        }

        m_variables_map.insert({it.key(),
                                {Variable::DIM_OR_VAR::VARIABLE, name, type, description, unit, options}});

      }

      auto dimensions_node = m_node["dimensions"];

      for (auto it = dimensions_node.begin(); it != dimensions_node.end(); ++it) {

        const std::string &name(it.key());
        auto type = (*it)["type_"].get<std::string>();
        auto description = (*it)["description"].get<std::string>();
        auto unit = (*it)["unit"].get<std::string>();

        std::vector<std::string> options;
        if ((*it).find("options") != (*it).end()) {
          options = (*it).get<std::vector<std::string>>();
        }

        m_dimensions_map.insert({it.key(),
                                 {Variable::DIM_OR_VAR::DIMENSION, name, type, description, unit, options}});

      }


    }


    int version() const { return m_version; }

    std::string dump(int indent = 2) const {
      return m_node.dump(indent);
    }

    std::string is_variable_recognized(const std::string &name) const {
      for (auto it = m_variables_map.begin(); it != m_variables_map.end(); ++it) {
        if (std::regex_match(name, std::regex(it->first))) {
          return it->first;
        }
      }
      return "";
    }

    bool is_this_list_consistent_with_version(const std::vector<std::string> &name_list,
                                              Variable::DIM_OR_VAR dim_or_var) const {
      for (const auto &name : name_list) {
        if (is_variable_recognized(name).empty()) return false;
      }
      return true;
    }

    Variable *get(const std::string &var_name) {
      auto key = is_variable_recognized(var_name);
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


  class PolarVersions {

   public:
    explicit PolarVersions(const fs::path &scheme_folder);

    int head_version() const { return m_head_version; }

    const PolarVersion &get(int version) const {
      check_version_bounds(version);
      return m_versions.at(version - 1);
    }

    bool is_this_list_consistent_with_version(const std::vector<std::string> &name_list,
                                              int version,
                                              Variable::DIM_OR_VAR dim_or_var) const {
      return get(version).is_this_list_consistent_with_version(name_list, dim_or_var);
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

  class NameMapper {
   public:
    NameMapper(int version_from, int version_to) {

    }

   private:
    std::unordered_map<std::string, std::string> m_mapper;
  };

}  // poem

#endif //POEM_POLARVERSIONS_H
