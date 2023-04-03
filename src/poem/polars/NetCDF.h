//
// Created by frongere on 01/02/2022.
//

#ifndef MSHIP_NETCDF_H
#define MSHIP_NETCDF_H

#include <string>
#include <vector>
#include <netcdf>
#include <boost/array.hpp>

#include <MathUtils/RegularGridInterpolator.h>
#include <MathUtils/RegularGridNearest.h>


namespace mship {

  template <typename T>
  std::vector<T> read_coord(const netCDF::NcFile &data_file, const std::string& coord_name);

  std::vector<std::string> read_coord_string(const netCDF::NcFile &data_file, const std::string& coord_name);

  namespace internal {

    template<typename T, size_t _dim>
    void load_polar(const netCDF::NcFile &dataFile,
                    const std::vector<std::string> &coords_names,
                    const std::string &var_name,
                    mathutils::RegularGridInterpolator<T, _dim> *interpolator) {

      assert(coords_names.size() == _dim);

      using Array_t = boost::multi_array<T, _dim>;
      boost::array<typename Array_t::index, _dim> shape;

      for (int i = 0; i < _dim; ++i) {
        auto coord_name = coords_names[i];
        auto coord = read_coord<T>(dataFile, coord_name);
        shape[i] = coord.size();
        interpolator->AddCoord(coord);
      }

      Array_t var(shape);
      auto ncvar = dataFile.getVar(var_name);
      ncvar.getVar(var.data());
      interpolator->AddVar(var);

    }

    template<size_t _dim>
    void load_solver_status(const netCDF::NcFile &dataFile,
                            const std::vector<std::string> &coords_names,
                            mathutils::RegularGridNearest<int, _dim, double> *nearest) {

      assert(coords_names.size() == _dim);

      using Array_t = boost::multi_array<int, _dim>;
      boost::array<typename Array_t::index, _dim> shape;

      for (int i = 0; i < _dim; ++i) {
        const auto &coord_name = coords_names[i];
        auto coord = read_coord<double>(dataFile, coord_name);
        shape[i] = coord.size();
        nearest->AddCoord(coord);
      }

      Array_t var(shape);
      auto ncvar = dataFile.getVar("SolverStatus");
      ncvar.getVar(var.data());
      nearest->AddVar(var);

    }



  }  // mship::internal

  template <typename T>
  std::vector<T> read_coord(const netCDF::NcFile &data_file, const std::string &coord_name) {
    auto coords_vars = data_file.getCoordVars();
    if (coords_vars.find(coord_name) == coords_vars.end()) {
      std::cerr << coord_name << " coord not found" << std::endl;
      exit(EXIT_FAILURE); // TODO: remplacer par une gestion des erreurs std::error_code...
    }

    auto coord_var = data_file.getVar(coord_name);
    auto coord_dim = coord_var.getDim(0);
    auto coord_size = coord_dim.getSize();
    std::vector<T> coord_vec(coord_size);
    coord_var.getVar(coord_vec.data());
    return coord_vec;
  }

}  // mship


#endif //MSHIP_NETCDF_H
