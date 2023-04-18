//
// Created by frongere on 14/04/23.
//

#include "Schema.h"

#include <spdlog/spdlog.h>


//#ifndef SCHEMA_TEST
#include "poem/schema/schema.h"
//#else
//
//#include "poem/schema/test/schema.h"
//
//#endif

#include <iostream>

#include "poem/exceptions.h"

namespace poem {

  Schema::Schema(const std::string &json_str) :
      m_json_str(json_str),
      m_json_schema(json::parse(json_str)) {
    load_global_attributes();

    // TODO: le reste (dimensions & variables)
    load_dimensions();
    load_variables();

  }

  const std::string &Schema::json_str() const {
    return m_json_str;
  }

  void Schema::check_attributes(Attributes *attributes) {
    /*
     * 1- on itere sur les attributs et on verifie qu'ils correspondent tous
     * 2- on itere sur les attributs du schema et on verifie que chaque attribut requis est bien la
     */

    for (const auto &attribute : *attributes) {
      if (m_global_attributes_map.find(attribute.first) == m_global_attributes_map.end()) {
        // Maybe the
        spdlog::critical("Attribute {} is not in the scheme", attribute.first);
        CRITICAL_ERROR
      }
    }




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

  const Schema GetCurrentSchema() {
    return Schema(schema::schema_str);
  }

}  // poem