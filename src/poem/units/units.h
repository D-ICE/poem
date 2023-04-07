//
// Created by frongere on 07/04/23.
//

#include <unordered_map>
#include <string>
//#include <ra>

#ifndef POEM_UNITS_H
#define POEM_UNITS_H

// Adapted from: https://github.com/hgrecco/pint/blob/master/pint/default_en.txt

namespace units {

  // Singleton design pattern
  class UnitsChecker {
   public:
    static UnitsChecker &getInstance() {
      static UnitsChecker instance;
      return instance;
    }

    UnitsChecker(const UnitsChecker &) = delete;

    void operator=(const UnitsChecker &) = delete;

    bool is_valid_unit(const std::string &unit, bool case_sensitive) const {
      for (const auto &unit_: m_units) {

        if (case_sensitive) {
          if (unit == unit_.first) return true;
        } else {
          if (to_lower(unit) == to_lower(unit_.first)) return true;
        }

      }

      return false;
    }

   private:
    UnitsChecker() :
        m_units{
            // No dimensions
            {"-",    "no dimension"},
            {"%",    "percent"},

            // distance
            {"m",    "meter"},
            {"cm",   "centimeter"},
            {"dm",   "decimeter"},
            {"km",   "kilometer"},
            {"nmi",  "nautical_mile"},

            // Area
            {"m2",   "meter_square"},

            // Volume
            {"m3",   "meter_cube"},

            // angle
            {"rad",  "radian"},
            {"deg",  "degree"},

            // time
            {"s",    "second"},
            {"min",  "minute"},
            {"hr",   "hour"},
            {"d",    "day"},

            // Frequency
            {"Hz",   "Hertz"},
            {"rpm",  "round_per_minute"},
            {"rps",  "round_per_second"},
            {"cps",  "count_per_seconds"},

            // velocity units
            {"kt",   "knot"},
            {"mph",  "mile_per_hour"},
            {"kph",  "kilometer_per_hour"},
            {"mps",  "meter_per_second"},

            // Acceleration units
            {"mps2", "meter_per_seconds_squared"},

            // mass
            {"g",    "gram"},
            {"kg",   "kilogram"},
            {"t",    "tonne"},

            // force units
            {"N",    "newton"},
            {"kN",   "kilonewton"},

            // Moments
            {"Nm",   "newton_meter"},
            {"kNm",  "kilonewton_meter"},

            // Energy
            {"J",    "joule"},
            {"Wh",   "watthour"},

            // Power
            {"W",    "watt"},
            {"kW",   "kilowatt"},
            {"VA",   "volt_ampere"},
            {"hp",   "horsepower"},

            // Pressure
            {"Pa",   "pascal"},
            {"bar",  "bar"},

            // temperature
            {"K",    "kelvin"},
            {"°C",   "degree_Celsius"},
            {"°F",   "degree_Farenheit"},

            // Current
            {"A",    "ampere"},
            {"mA",   "miliampere"},

            // Charge
            {"C",    "coulomb"},
            {"Ah",   "apere_hour"},

            // Electric potential
            {"V",    "volt"},

            // Resistance
            {"ohm",  "ohm"},

        } {}

    static inline std::string to_lower(const std::string &in) {
      auto _in = in;
      std::transform(_in.begin(), _in.end(), _in.begin(), [](unsigned char c) { return std::tolower(c); });
      return _in;
    }

   private:
    std::unordered_map<std::string, std::string> m_units;

  };

}  // units

#endif //POEM_UNITS_H
