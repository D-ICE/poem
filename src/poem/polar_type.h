//
// Created by frongere on 17/05/24.
//

#ifndef MSHIP_POLARTYPE_H
#define MSHIP_POLARTYPE_H

#include <string>
#include <spdlog/spdlog.h>

#include "exceptions.h"

namespace poem
{

  enum POLAR_TYPE
  {
    PPP,  // STW Control, motor only
    HPPP, // STW Control, hybrid motor and wind propulsion
    HVPP, // BrakePower Control, hybrid motor and wind propulsion
    MVPP, // BrakePower Control, motor only
    VPP,  // No Control, wind propulsion only
  };

  inline POLAR_TYPE polar_type_s2enum(const std::string &polar_type_)
  {
    POLAR_TYPE polar_type;
    if (polar_type_ == "PPP")
    {
      polar_type = PPP;
    }
    else if (polar_type_ == "HPPP")
    {
      polar_type = HPPP;
    }
    else if (polar_type_ == "HVPP")
    {
      polar_type = HVPP;
    }
    else if (polar_type_ == "MVPP")
    {
      polar_type = MVPP;
    }
    else if (polar_type_ == "VPP")
    {
      polar_type = VPP;
    }
    else if (polar_type_ == "ND")
    {
      // This is the v0 version of POEM specs...
      polar_type = HPPP;
    }
    else
    {
      spdlog::critical("Polar type \"{}\" unknown", polar_type);
      CRITICAL_ERROR_POEM
    }
    return polar_type;
  }

  inline std::string polar_type_enum2s(const POLAR_TYPE &polar_type_)
  {
    std::string polar_type;
    switch (polar_type_)
    {
    case PPP:
      polar_type = "PPP";
      break;
    case HPPP:
      polar_type = "HPPP";
      break;
    case HVPP:
      polar_type = "HVPP";
      break;
    case MVPP:
      polar_type = "MVPP";
      break;
    case VPP:
      polar_type = "VPP";
      break;
    }
    return polar_type;
  }

} // poem

#endif // MSHIP_POLARTYPE_H
