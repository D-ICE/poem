//
// Created by frongere on 11/04/23.
//

#ifndef POEM_POLARWRITER_H
#define POEM_POLARWRITER_H

//#include "poem/schema/schema.h"
#include "poem/exceptions.h"

namespace poem {

  class PolarSet;

  class PolarWriter {

   public:
    explicit PolarWriter(PolarSet *polar_set) :
        m_polar_set(polar_set) {};

    int write(const std::string &nc_file) const;

   private:
    PolarSet *m_polar_set;
//    json m_schema;

  };


}  // poem


#endif //POEM_POLARWRITER_H
