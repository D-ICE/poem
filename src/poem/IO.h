//
// Created by frongere on 17/01/25.
//

#ifndef POEM_IO_H
#define POEM_IO_H

#include <netcdf>
#include <filesystem>

#include "exceptions.h"
#include "enums.h"
#include "Attributes.h"

namespace fs = std::filesystem;

namespace poem {

  // Forward declarations
  class PolarTableBase;

  template<typename T>
  class PolarTable;

  class DimensionGrid;

  class Polar;

  class PolarSet;

  class PolarNode;


  // ===================================================================================================================
  // WRITERS
  // ===================================================================================================================

  template<typename T>
  void to_netcdf(std::shared_ptr<PolarTable<T>> polar_table,
                 const netCDF::NcType &nc_type,
                 netCDF::NcGroup &group);

  int current_poem_standard_version();

  std::vector<netCDF::NcDim> write_dimension_grid(std::shared_ptr<DimensionGrid> dimension_grid,
                                                  netCDF::NcGroup &group);

  void to_netcdf(const Attributes &attributes, netCDF::NcGroup &group);

  void to_netcdf(const Attributes &attributes, netCDF::NcVar &nc_var);

  void to_netcdf(std::shared_ptr<Polar> polar, netCDF::NcGroup &group);

  void to_netcdf(std::shared_ptr<PolarSet> polar_set, netCDF::NcGroup &group);

  void to_netcdf(std::shared_ptr<PolarNode> polar_node, netCDF::NcGroup &group);

  void to_netcdf(std::shared_ptr<PolarNode> polar_node,
                 const std::string &vessel_name,
                 const std::string &filename,
                 bool verbose = true);

  // ===================================================================================================================
  // READERS
  // ===================================================================================================================

  int get_version(const std::string &filename);

  std::shared_ptr<PolarNode> load_v0(const netCDF::NcGroup &root_group);

  std::shared_ptr<PolarNode> load_v1(const netCDF::NcGroup &root_group);

  std::shared_ptr<PolarNode> load(const std::string &filename,
                                  bool spec_checking = true,
                                  bool verbose = true);

}  // poem

#include "IO.inl"

#endif //POEM_IO_H
