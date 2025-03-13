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
  void to_netcdf(const std::shared_ptr<PolarTable<T>> &polar_table,
                 const netCDF::NcType &nc_type,
                 const netCDF::NcGroup &group,
                 const std::string &filename);

  int current_poem_standard_version();

  std::vector<netCDF::NcDim> write_dimension_grid(const std::shared_ptr<DimensionGrid> &dimension_grid,
                                                  const netCDF::NcGroup &group,
                                                  const std::string &filename);

  void to_netcdf(const Attributes &attributes,
                 const netCDF::NcGroup &group,
                 const std::string &filename);

  void to_netcdf(const Attributes &attributes,
                 const netCDF::NcVar &nc_var,
                 const std::string &filename);

  void to_netcdf(const std::shared_ptr<Polar> &polar,
                 const netCDF::NcGroup &group,
                 const std::string &filename);

  void to_netcdf(const std::shared_ptr<PolarSet> &polar_set,
                 const netCDF::NcGroup &group,
                 const std::string &filename);

  void to_netcdf(const std::shared_ptr<PolarNode> &polar_node,
                 const netCDF::NcGroup &group,
                 const std::string &filename);

  void to_netcdf(const std::shared_ptr<PolarNode> &polar_node,
                 const std::string &vessel_name,
                 const std::string &filename,
                 bool verbose = true);

  // ===================================================================================================================
  // READERS
  // ===================================================================================================================

  int get_version(const std::string &filename);

  std::shared_ptr<PolarNode> load_v0(const netCDF::NcGroup &root_group, const std::string &filename);

  std::shared_ptr<PolarNode> load_v1(const netCDF::NcGroup &root_group, const std::string &filename);

  std::shared_ptr<PolarNode> load(const std::string &filename,
                                  bool spec_checking = true,
                                  bool verbose = true);

}  // poem

#include "IO.inl"

#endif //POEM_IO_H
