//
// Created by frongere on 03/04/23.
//

#include <iostream>


#include "poem/poem.h"

#include "poem/units/units.h"  // TODO: retirer
#include "poem/exceptions.h"


using namespace poem;

int main() {

  std::cout << units::UnitsChecker::getInstance().is_valid_unit("kn", false) << std::endl;

  return 1;

//  units.is_valid_unit()


  PolarVersions scheme(SCHEME_PATH);

  auto v1 = scheme.get(1);

//  std::cout << v1.is_variable_recognized("main_ActivePower") << std::endl;

//  std::string var_name("main_ActivePower");
  std::string var_name("manoeuvring_K");
  auto var = v1.get(var_name);
  if (var) {
    std::cout << var->name() << std::endl;
    std::cout << var->type() << std::endl;
    std::cout << var->description() << std::endl;
    std::cout << var->unit() << std::endl;
  } else {
    std::cout << var_name << " is not recognized" << std::endl;
  }

//  std::string var_name("main_ActivePower");
//  if (std::regex_match(var_name, std::regex(R"(\w+_ActivePower)"))) {
//    std::cout << "Match" << std::endl;
//  } else {
//    std::cout << "NO MATCH" << std::endl;
//  }

  return 0;
}