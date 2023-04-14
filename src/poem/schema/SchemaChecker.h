//
// Created by frongere on 14/04/23.
//

#ifndef POEM_SCHEMACHECKER_H
#define POEM_SCHEMACHECKER_H

#ifndef SCHEMA_TEST

#include "poem/schema/schema.h"

#else

#include "poem/schema/test/schema_test.h"

#endif

#include "Schema.h"

namespace poem {

  // Forward declaration
  class PolarSet;

  class SchemaChecker {
   public:
    static SchemaChecker &getInstance() {
      static SchemaChecker instance;
      return instance;
    }

    SchemaChecker(const SchemaChecker &) = delete;

    void operator=(const SchemaChecker &) = delete;

   public:
    void check(const PolarSet* polar_set);

   private:
    #ifndef SCHEMA_TEST
    SchemaChecker() : m_schema(schema::schema) {
      m_schema = std::make_unique<Schema>(m_json_schema);
    }
    #else

    SchemaChecker() : m_json_schema(schema_test::schema_test) {
      m_schema = std::make_unique<Schema>(m_json_schema);
    }

    #endif

   private:
    json m_json_schema;
    std::unique_ptr<Schema> m_schema;

  };

}  // poem

#endif //POEM_SCHEMACHECKER_H
