//
// Created by frongere on 13/04/23.
//

#include "poem/poem.h"

using namespace poem;

int main() {

  std::string nc_file("essai.nc");

  PolarReader reader;

  reader.Read(nc_file);



  return 0;
}