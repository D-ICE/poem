//
// Created by frongere on 14/04/23.
//

#ifndef POEM_SCHEMA_H
#define POEM_SCHEMA_H

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace poem {

  class Schema {
   public:
    explicit Schema(const json& json_schema);

   private:
    json m_json_schema;

  };

}  // poem

#endif //POEM_SCHEMA_H
