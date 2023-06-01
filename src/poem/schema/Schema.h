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

#define KNOWN_SCHEMA_ELEMENT_FIELDS {"doc", "type", "tags", "dimensions", "aliases"}
#define REQUIRED_SCHEMA_ELEMENT_FIELDS {"doc", "type"}

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace poem {

  bool glob_match(const char *text, const char *glob);

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

    bool is_alias(const std::string &name) const {
      return std::find(m_aliases.begin(), m_aliases.end(), name) != m_aliases.end();
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
    Schema(const std::string &json_str, bool is_newest);

    bool operator==(const Schema &other) const;

    std::string json_str(const int indent=-1) const;

    bool is_newest() const;

    const SchemaElement &get_attribute_schema(const std::string &name) const;

    const SchemaVariable &get_variable_schema(const std::string &name) const;

    void check_attributes(Attributes *attributes) const;

    template<typename T, size_t _dim>
    void check_polar(Polar<T, _dim> *polar) const;

    std::string get_newest_attribute_name(const std::string &name) const;

    std::string get_newest_variable_name(const std::string &name) const;

   private:
    void load_global_attributes();

    void load_dimensions();

    void load_variables();

   protected:
    mutable bool m_is_newest;

    json m_json_schema;

    std::unordered_map<std::string, SchemaElement> m_global_attributes_map;

    std::unordered_map<std::string, std::string> m_dimensions_attributes_map;
    std::unordered_map<std::string, SchemaElement> m_dimensions_map;

    std::unordered_map<std::string, std::string> m_variables_attributes_map;
    std::unordered_map<std::string, SchemaVariable> m_variables_map;

    std::vector<std::string> m_regexes;

    // TODO: faire map de dimensions et de variables aussi

  };

  Schema get_newest_schema();

}  // poem

#include "Schema.hpp"

#endif //POEM_SCHEMA_H
