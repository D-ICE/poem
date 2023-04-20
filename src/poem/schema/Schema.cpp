//
// Created by frongere on 14/04/23.
//

#include "Schema.h"

#include <spdlog/spdlog.h>


#include "poem/polar/Attributes.h"
#include "poem/polar/Polar.h"

#include "poem/schema/schema.h"

#include <iostream>

#include "poem/exceptions.h"

namespace poem {

  Schema::Schema(const std::string &json_str, bool check_is_last) :
      m_json_str(json_str),
      m_json_schema(json::parse(json_str)),
      m_is_last(false) {

    if (check_is_last) {
      m_is_last = *this == LastSchema::getInstance();
    }

    load_global_attributes();
    load_dimensions();
    load_variables();

  }

  bool Schema::operator==(const Schema &other) const {
    return m_json_schema == other.m_json_schema;
  }

  const std::string &Schema::json_str() const {
    return m_json_str;
  }

  bool Schema::is_last() const {
    return m_is_last;
  }

  const GlobalAttributeSchema &Schema::get_attribute_schema(const std::string &name) const {
    return m_global_attributes_map.at(name);
  }

  void Schema::check_attributes(Attributes *attributes) const {
    /*
     * 1- on itere sur les attributs et on verifie qu'ils correspondent tous
     * 2- on itere sur les attributs du schema et on verifie que chaque attribut requis est bien la
     *
     * attributes sont les noms writer
     *
     * schema atts sont les noms reader (derniere version donc)
     *
     * on veut faire de la map reader -> writer et dans reader, on doit trouver un alias qui va bien
     *
     * on est cense de fait toujours avoir version reader >= version writer
     *
     */

    // this->m_schema, c'est le schema d'ecriture de la polaire
    // LastSchema::getInstance() c'est le schema de lecture (le dernier, toujours)

    // Au write, on veut pouvoir checker que les champs donnes sont compliant avec le dernier schema
    //  pour pouvoir faire evoluer le schema au besoin

    // Au read, on veut pouvoir mettre en relation le schema read (le dernier) et le schema write qui avait ete utilise


    // Are the attributes consistent with the given schema?
    for (const auto &attribute: *attributes) {
      if (m_global_attributes_map.find(attribute.first) == m_global_attributes_map.end()) {
        // Maybe the
        spdlog::critical("Attribute \"{}\" is not in the scheme", attribute.first);
        CRITICAL_ERROR
      }
    }

    // Are every required attributes from schema present in attributes
    for (const auto &schema_atts: m_global_attributes_map) {
      if (schema_atts.second.is_required() && !attributes->contains(schema_atts.first)) {

        // Ok, not found with this name... looking for the aliases

        spdlog::critical("Required attribute \"{}\" not found in polar", schema_atts.first);
        CRITICAL_ERROR
      }
    }

  }

  void Schema::check_polar(PolarBase *polar) const {
    //TODO
  }

  std::string Schema::get_current_attribute_name(const std::string &name) const {
    std::string current_name;

    if (m_global_attributes_map.find(name) != m_global_attributes_map.end()) {
      current_name = name;

    } else {

//      bool found = false;
      for (const auto& attribute : m_global_attributes_map) {
        auto aliases = attribute.second.aliases();
        auto iter = std::find(aliases.begin(), aliases.end(), name);
        if (iter != aliases.end()) {
          // name has been found in aliases
//          found = true;
          current_name = *iter;
          break;
        }
      }

//      if (!found) {
//        spdlog::critical("No backward compatibility between schema for attribute named {}", name);
//        CRITICAL_ERROR;
//      }

    }

    return current_name;
  }

  void Schema::load_global_attributes() {
    auto node = m_json_schema["global_attributes"];
    for (auto iter = node.begin(); iter != node.end(); ++iter) {
      GlobalAttributeSchema attribute(iter.value());
      m_global_attributes_map.insert({iter.key(), attribute});
    }
  }

  void Schema::load_dimensions() {
    // TODO
  }

  void Schema::load_variables() {
    // TODO
  }





//  const Schema GetCurrentSchema() {
//    return Schema(schema::schema_str);
//  }

  LastSchema &LastSchema::getInstance() {
    static LastSchema instance;
    return instance;
  }

  LastSchema::LastSchema() : Schema(schema::schema_str, false) {
    m_is_last = true;
  }

}  // poem