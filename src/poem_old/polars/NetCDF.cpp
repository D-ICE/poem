//
// Created by frongere on 01/02/2022.
//

#include "NetCDF.h"

namespace mship {

  std::vector<std::string> read_coord_string(const netCDF::NcFile &data_file, const std::string &coord_name) {
    auto coord_var = data_file.getVar(coord_name);
    auto coord_dim = coord_var.getDim(0);
    auto coord_size = coord_dim.getSize();

    std::vector<std::string> coord_vec;
    coord_vec.reserve(coord_size);

    // Reding string into file
    std::vector<char*> coord_vec_tmp(coord_size);
    coord_var.getVar(coord_vec_tmp.data());

    // Building a proper string vector
    for (const auto& cmp : coord_vec_tmp) {
      coord_vec.emplace_back(std::string(cmp));
    }

    return coord_vec;
  }

}  // mship

