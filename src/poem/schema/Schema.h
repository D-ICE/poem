//
// Created by frongere on 14/04/23.
//

#ifndef POEM_SCHEMA_H
#define POEM_SCHEMA_H

#include <nlohmann/json.hpp>

#include "poem/polar/Attributes.h"

using json = nlohmann::json;

namespace poem {

  /**
   * Liste des tags possibles
   * required (si pas specifie, on est optional...)
   * deprecated (la variable n'est plus lisible, elle a ete supprimee)
   *
   *
   */

  class GlobalAttributeSchema {
   public:
    explicit GlobalAttributeSchema(const json &node) :
        m_required(false),
        m_deprecated(false) {

      m_type = node["type"].get<std::string>();

      // TODO: faire de methodes statiques pour les recherche...
      auto tags = get<std::vector<std::string>>(node, "tags");

      for (const auto &tag: tags) {
        if (tag == "required") m_required = true;
        if (tag == "deprecated") m_deprecated = true;
      }

      m_aliases = get<std::vector<std::string>>(node, "aliases");

    }

    bool is_required() const { return m_required; }

    bool is_deprecated() const { return m_deprecated; }

    std::vector<std::string> aliases() const { return m_aliases; }


    template<typename T>
    static T get(const json &node, const std::string &key) {
      T val;
      if (node.find(key) != node.end()) {
        val = node[key].get<T>();
      }
      return val;
    }

   private:
    std::string m_type;
    bool m_required;
    bool m_deprecated;
    std::vector<std::string> m_aliases;

  };

  class Schema {
   public:
    explicit Schema(const std::string &json_str);

    bool operator==(const Schema& other) const {
      return m_json_schema == other.m_json_schema;
    }

    const std::string &json_str() const;

    void check_attributes(Attributes* attributes);

   private:
    void load_global_attributes();

    void load_dimensions();

    void load_variables();

   protected:
    std::string m_json_str;

    json m_json_schema;

    std::unordered_map<std::string, GlobalAttributeSchema> m_global_attributes_map;

  };


  class LastSchema : public Schema {
   public:
    static Schema &getInstance();

    LastSchema(const LastSchema &) = delete;
    void operator=(const LastSchema &) = delete;

   private:
    LastSchema();

  };


}  // poem

#endif //POEM_SCHEMA_H
