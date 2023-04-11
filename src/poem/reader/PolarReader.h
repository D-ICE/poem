//
// Created by frongere on 11/04/23.
//

#ifndef POEM_POLARREADER_H
#define POEM_POLARREADER_H

#include <iostream>
#include <string>

#include "poem/polar/Polar.h"

namespace poem {

  class PolarReader {
   public:
    PolarReader() {}

    POLAR_TYPE Read(const std::string &nc_polar);

   private:
    int m_polar_version;


  };


}  // poem

#endif //POEM_POLARREADER_H
