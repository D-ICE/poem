//
// Created by frongere on 11/04/23.
//

#include "PolarWriter.h"

#include <iostream>

#include "poem/polar/Polar.h"

namespace poem {

  void PolarWriter::write(const std::string &nc_file) const {


//    for (const auto &polar: *m_polar_set) {
    for (auto iter=m_polar_set->begin(); iter!=m_polar_set->end(); ++iter) {
//      auto variable_ID = polar->variable_ID();
      auto polar = (*iter).get();

//      auto type = polar->variable_ID()->type();
//      std::cout << polar->variable_ID()->name() << std::endl;

      /*
       * On voudrait pouvoir recuperer le dim_ID_array qui definit la signature des dimensions de la polaire
       *
       * On veut recuperer les points de polaire
       *
       *
       *
       */


//      auto polar_ = static_cast<Polar<double, 3>*>(polar);

//      for (const auto &point : *polar) {
//
//      }




      int  i =1;


    }

//    STOP
  }


}  // poem
