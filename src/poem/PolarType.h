//
// Created by frongere on 17/05/24.
//

#ifndef MSHIP_POLARTYPE_H
#define MSHIP_POLARTYPE_H

#include <string>
#include <spdlog/spdlog.h>

namespace poem {

  enum POLAR_TYPE {
    PPP,
    HPPP,
    HVPP_PB,
    HVPP_HP,
    MVPP,
    VPP,
  };

  inline POLAR_TYPE polar_type_s2enum(const std::string& polar_type_) {
    POLAR_TYPE polar_type;
    if (polar_type_ == "PPP") {
      polar_type = PPP;
    } else if (polar_type_ == "HPPP") {
      polar_type = HPPP;
    } else if (polar_type_ == "HVPP_PB") {
      polar_type = HVPP_PB;
    } else if (polar_type_ == "HVPP_HP") {
      polar_type = HVPP_HP;
    } else if (polar_type_ == "MVPP") {
      polar_type = MVPP;
    } else if (polar_type_ == "VPP") {
      polar_type = VPP;
    } else if (polar_type_ == "ND") {
      // This is the v0 version of POEM specs...
      polar_type = HPPP;
    } else {
      spdlog::critical("Polar type \"{}\" unknown", polar_type);
      CRITICAL_ERROR_POEM
    }
    return polar_type;
  }

}  // poem

#endif //MSHIP_POLARTYPE_H
