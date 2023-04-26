//
// Created by frongere on 14/04/23.
//

#ifndef POEM_SCHEMA_H
#define POEM_SCHEMA_H

#include <filesystem>
#include <nlohmann/json.hpp>

// TODO: mettre dans le .cpp
#include <spdlog/spdlog.h>
#include "poem/exceptions.h"
// TODO FIN

#define KNOWN_SCHEMA_ELEMENT_FIELDS {"doc", "type", "tags", "dimensions"}
#define REQUIRED_SCHEMA_ELEMENT_FIELDS {"doc", "type"}

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace poem {

  template<typename, size_t>
  class Polar;

  /**
   * Liste des tags possibles
   * required (si pas specifie, on est optional...)
   * deprecated (la variable n'est plus lisible, elle a ete supprimee)
   *
   *
   */




  /**
   * SchemaElement defines a general field of a PolarSet (global attributes, dimensions or variables)
   *
   * This is where we define the logic for schema evolution
   */
  class SchemaElement {

   public:
    explicit SchemaElement(const std::string &name, const json &node) :
        m_name(name),
        m_required(false),
        m_deprecated(false) {

      static std::vector<std::string> required = REQUIRED_SCHEMA_ELEMENT_FIELDS;

      // Every required fields must be present...
      for (const auto &field: required) {
        if (node.find(field) == node.end()) {
          spdlog::critical(R"(In "{}" schema element definition, field "{}" is required)", m_name, field);
          CRITICAL_ERROR
        }
      }

//       TODO // tester que type est connu

      if (get<std::string>(node, "doc").empty()) {
        spdlog::warn(
            R"(Hey!! When you add a new field definition in the schema (here "{}"), please provide a non empty and real doc)",
            m_name);
      }


      // Tester si les champs requis sont presents (type, doc) -> on force a ecrire un schema de qualite
      // FIXME: si type ou autre n'est pas present, les messages d'erreur ne sont pas suffisamment explicites

      m_type = node["type"].get<std::string>();  // TODO: verifier que type est bien connu...

      // TODO: faire de methodes statiques pour les recherche...
      auto tags = get<std::vector<std::string>>(node, "tags");

      for (const auto &tag: tags) {
        if (tag == "required") {
          m_required = true;
        } else if (tag == "deprecated") {
          m_deprecated = true;
        } else {
          spdlog::critical(R"(In "{}" schema element tags definition, tag "{}" is unknown)", m_name, tag);
          CRITICAL_ERROR
        }
      }

      m_aliases = get<std::vector<std::string>>(node, "aliases");

      // TODO: eventuellement tester qu'on a pas d'autres champs inconnus pour ne pas mettre n'importe quoi dans le
      //  schema
      std::vector<std::string> known_fields = KNOWN_SCHEMA_ELEMENT_FIELDS;
      for (auto iter = node.begin(); iter != node.end(); ++iter) {
        auto key = iter.key();
        if (key[0] == '_') continue;  // fields that start with underscore '_' are hidden
        if (std::find(known_fields.begin(), known_fields.end(), key) == known_fields.end()) {
          spdlog::warn(R"(In "{}" schema element, field "{}" is not a known field)", m_name, iter.key());
        }
      }

    }

    bool is_required() const { return m_required; }

    bool is_deprecated() const { return m_deprecated; }

    std::string get_alias(const std::string &name) const {
      std::string alias;
      auto iter = std::find(m_aliases.begin(), m_aliases.end(), name);
      if (iter != m_aliases.end()) {
        alias = *iter;
      }
      return alias;
    }


    template<typename T>
    static T get(const json &node, const std::string &key) {
      T val;
      if (node.find(key) != node.end()) {
        val = node[key].get<T>();
      }
      return val;
    }

   protected:
    std::string m_name;
    std::string m_type;
    bool m_required;
    bool m_deprecated;
    std::vector<std::string> m_aliases;
  };

  class SchemaVariable : public SchemaElement {
   public:
    explicit SchemaVariable(const std::string &name, const json &node) :
        SchemaElement(name, node),
        m_dimensions_names(get<std::vector<std::string>>(node, "dimensions")) {

      // Dimensions field must be present for variables
      if (node.find("dimensions") == node.end()) {
        spdlog::critical(R"("dimensions" fields must be present in variable schema definition (in variable "{}"))",
                         name);
        CRITICAL_ERROR
      }

    }

    const std::vector<std::string> &dimensions_names() const { return m_dimensions_names; }

   private:
    std::vector<std::string> m_dimensions_names;
  };


  // Forward declaration
  class Attributes;

  class PolarBase;

  class Schema {
   public:
    Schema(const std::string &json_str, bool check_is_last = true);

    bool operator==(const Schema &other) const;

    std::string json_str() const;

    bool is_last() const;

    const SchemaElement &get_attribute_schema(const std::string &name) const;

    const SchemaVariable &get_variable_schema(const std::string &name) const;

    void check_attributes(Attributes *attributes) const;

    template<typename T, size_t _dim>
    void check_polar(Polar<T, _dim> *polar) const;

    std::string get_current_attribute_name(const std::string &name) const;

    std::string get_current_variable_name(const std::string &name) const;

   private:
    void load_global_attributes();

    void load_dimensions();

    void load_variables();

   protected:
    mutable bool m_is_last;
//    std::string m_json_str;

    json m_json_schema;

    std::unordered_map<std::string, SchemaElement> m_global_attributes_map;

    std::unordered_map<std::string, std::string> m_dimensions_attributes_map;
    std::unordered_map<std::string, SchemaElement> m_dimensions_map;

    std::unordered_map<std::string, std::string> m_variables_attributes_map;
    std::unordered_map<std::string, SchemaVariable> m_variables_map;

    // TODO: faire map de dimensions et de variables aussi

  };

  /**
   * Exactly the same as Schema but reserved for the very last version of the schema in poem library
   *
   * Follows a Singleton pattern to get only one instance at runtime
   */
  class LastSchema : public Schema {
   public:
    static LastSchema &getInstance();

//    #ifdef TEST_SCHEMAS_DIR
//    void set
//    #endif

    LastSchema(const LastSchema &) = delete;

    void operator=(const LastSchema &) = delete;

   private:
    LastSchema();

  };


  template<typename T, size_t _dim>
  void Schema::check_polar(Polar<T, _dim> *polar) const {

    auto polar_name = polar->name();

    /*
     * FIXME: Doit-on empecher de mettre des choses qui ne sont pas connues par le schema ??
     *  Doit'on mettre une option de check strict ?
     */

    // Check if polar_name is present in variables map
    if (m_variables_map.find(polar_name) == m_variables_map.end()) {
      // The variable is not known
      spdlog::critical(R"(Variable "{}" is not known by the given schema)", polar_name);
      CRITICAL_ERROR
    }

    // Pas possible de check ici que les variables requises par le scheme sont toutes la
    // Il faut une methode speciale de PolarSet qui declenche le check une fois qu'on est certain
    // qu'on a identifie toutes les variables qui seront ajoutees


    // Maintentant il faut check la consistance de dimensions...

    // On veut recuperer le type et la dimension de la variable

    auto dimension_ID_set = polar->dimension_point_set()->dimension_ID_set();
    // On veut verifier que toutes les dimensions sont bien existantes
    for (size_t i = 0; i < _dim; ++i) {
      auto dim_ID = dimension_ID_set->get(i);
      auto dim_name = dim_ID->name();

      if (m_dimensions_map.find(dim_name) == m_dimensions_map.end()) {
        spdlog::critical(R"(Variable "{}" is said to depend on dimension "{}" which is not known by the schema)",
                         polar_name, dim_name);
        CRITICAL_ERROR
      }

    }

  }


}  // poem

#endif //POEM_SCHEMA_H
