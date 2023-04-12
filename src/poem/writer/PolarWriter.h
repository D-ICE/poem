//
// Created by frongere on 11/04/23.
//

#ifndef POEM_POLARWRITER_H
#define POEM_POLARWRITER_H

#include "poem/schema/schema.h"
#include "poem/exceptions.h"

namespace poem {

  class PolarBase;

  class PolarWriter {

   public:
    explicit PolarWriter(PolarBase* polar) :
    m_polar(polar),
    m_schema(schema::schema) {};

    void write(const std::string &nc_file) const {

      /*
       * Pseudo code
       *
       *
       *
       */

      NIY
    }

   private:
    PolarBase* m_polar;
    json m_schema;

  };




}  // poem


#endif //POEM_POLARWRITER_H
