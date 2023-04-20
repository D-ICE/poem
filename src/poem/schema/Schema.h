//
// Created by frongere on 14/04/23.
//

#ifndef POEM_SCHEMA_H
#define POEM_SCHEMA_H

#include <nlohmann/json.hpp>

// TODO: mettre dans le .cpp
#include <spdlog/spdlog.h>
#include "poem/exceptions.h"
// TODO FIN


using json = nlohmann::json;

namespace poem {

  /**
   * Liste des tags possibles
   * required (si pas specifie, on est optional...)
   * deprecated (la variable n'est plus lisible, elle a ete supprimee)
   *
   *
   */

  #define REQUIRED_SCHEMA_ELEMENT_FIELDS {"doc", "type"}

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

      std::vector<std::string> required = REQUIRED_SCHEMA_ELEMENT_FIELDS;

      for (const auto &field: required) {
        if (node.find(field) == node.end()) {
          spdlog::critical(R"(In {} schema element definition, field {} is required)", m_name, field);
          CRITICAL_ERROR
        }
      }

      // TODO:
      //  tester que doc n'est pas vide (forcer a commenter dans le schema !!)
      //  tester que type est connu


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
          spdlog::critical(R"(In {} schema element definition, tag {} is unknown)", m_name, tag);
          CRITICAL_ERROR
        }
      }

      m_aliases = get<std::vector<std::string>>(node, "aliases");

      // TODO: eventuellement tester qu'on a pas d'autres champs inconnus pour ne pas mettre n'importe quoi dans le
      //  schema

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
        m_dimensions_names(get<std::vector<std::string>>(node, "dimensions")) {}

    const std::vector<std::string> &dimensions_names() const { return m_dimensions_names; }

   private:
    std::vector<std::string> m_dimensions_names;
  };


  // Forward declaration
  class Attributes;

  class PolarBase;

  class Schema {
   public:
    explicit Schema(const std::string &json_str, bool check_is_last = true);

    bool operator==(const Schema &other) const;

    const std::string &json_str() const;

    bool is_last() const;

    const SchemaElement &get_attribute_schema(const std::string &name) const;

    void check_attributes(Attributes *attributes) const;

    void check_polar(PolarBase *polar) const;

    std::string get_current_attribute_name(const std::string &name) const;

   private:
    void load_global_attributes();

    void load_dimensions();

    void load_variables();

   protected:
    mutable bool m_is_last;
    std::string m_json_str;

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

    LastSchema(const LastSchema &) = delete;

    void operator=(const LastSchema &) = delete;

   private:
    LastSchema();

  };


}  // poem

#endif //POEM_SCHEMA_H
