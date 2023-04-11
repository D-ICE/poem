//
// Created by frongere on 11/04/23.
//

#ifndef POEM_POLARWRITER_H
#define POEM_POLARWRITER_H

namespace poem {

  class PolarWriter {
   public:
    explicit PolarWriter(int version) : m_version(version) {};

   private:
    int m_version;
  };




}  // poem


#endif //POEM_POLARWRITER_H
