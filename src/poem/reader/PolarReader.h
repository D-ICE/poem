//
// Created by frongere on 11/04/23.
//

#ifndef POEM_POLARREADER_H
#define POEM_POLARREADER_H

#include <iostream>
#include <string>



namespace poem {

  // Forward declaration
  class PolarSet;

  class PolarReader {
   public:
    PolarReader() {}

    std::shared_ptr<PolarSet> Read(const std::string &nc_polar);

   private:
    int m_polar_version;


  };


}  // poem

#endif //POEM_POLARREADER_H
