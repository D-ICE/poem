//
// Created by frongere on 11/04/23.
//

#ifndef POEM_DIMENSIONS_H
#define POEM_DIMENSIONS_H

#include <vector>
#include <string>
#include <array>

#include "poem/exceptions.h"

namespace poem {

  class Dimension {
   public:
    using Values = std::vector<double>;
    using Iter = Values::const_iterator;

    Dimension(const std::string &name, const Values &values, int version) :
        m_name(name),
        m_values(values),
        m_version(version) {
      // TODO: check name VS version
    }

    Iter begin() const { return m_values.cbegin(); }

    Iter end() const { return m_values.cend(); }

   private:
    int m_version;
    std::string m_name;
    Values m_values;

  };


  template<size_t dim_>
  class Dimensions {

   public:
    explicit Dimensions(int version) :
        m_version(version),
        m_nb_dim(0),
        m_dimensions({}){
      // TODO: check la coherence des noms par rapport a la version
    }

    void push_back(const std::string &name) {
      NIY
    }


   private:
    int m_nb_dim;

    int m_version;
    std::array<Dimension, dim_> m_dimensions;

  };


}  // poem

#endif //POEM_DIMENSIONS_H
