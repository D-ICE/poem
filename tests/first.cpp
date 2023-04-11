//
// Created by frongere on 08/04/23.
//

#include <iostream>

#include "poem/poem.h"
#include "spdlog/spdlog.h"

#define POLAR_VERSION 1

using namespace poem;

int main() {

  int version = 1;

//  spdlog::set_level(spdlog::level::critical); // Only critical
  spdlog::set_level(spdlog::level::trace);

  /*
   * 1- on cree un reader de polaire
   * 2- on lui donne un fichier .nc et il sort une polaire
   */

  PolarReader reader;

  std::string polar_file(std::string(DATA_FOLDER) + "/polar_5D_STW_v1.nc");
  std::cout << polar_file << std::endl;

  auto polar = reader.Read(polar_file);


//  Dimensions<5> dimensions({"STW_kt", "TWS_kt", "TWA_deg", "WA_deg", "Hs_m"}, version);



  return 0;
}