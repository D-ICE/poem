//
// Created by frongere on 17/01/25.
//

#ifndef POEM_IO_H
#define POEM_IO_H

#include <netcdf>
#include <filesystem>

#include "exceptions.h"

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

  int get_current_spec_version();

  namespace internal {

    template<typename T>
    void write_polar_table_(std::shared_ptr<PolarTable<T>> polar_table,
                            const netCDF::NcType &nc_type,
                            netCDF::NcGroup &group);
  }  // internal

  /**
   * Writes a PolarTableBase into a NetCDF group (non templated)
   */
  void write_polar_table(std::shared_ptr<PolarTableBase> polar_table, netCDF::NcGroup &group);

  void write_polar(std::shared_ptr<Polar> polar, netCDF::NcGroup &group);

  void write_polar_set(std::shared_ptr<PolarSet> polar_set, netCDF::NcGroup &group);

  void write_polar_node(std::shared_ptr<PolarNode> polar_node, netCDF::NcGroup &group);

  void to_netcdf(std::shared_ptr<PolarNode> polar_node, netCDF::NcGroup &group);

  void to_netcdf(std::shared_ptr<PolarNode> polar_node, const std::string& filename);

//  void from_netcdf(const netCDF::NcGroup &group);


  // ===================================================================================================================
  // READERS
  // ===================================================================================================================

  int get_version_from_nc_file(const std::string &filename);

  std::shared_ptr<DimensionGrid> read_dimension_grid_from_var(const netCDF::NcVar &var);

  /**
   * Read a NetCDF variable and returns it as a PolarTableBase
   *
   * @param var the NetCDF variable
   * @param dimension_grid reference to the DimensionGrid to be used
   * @param dimension_grid_from_var if true, the DimensionGrid is built from the variable,
   *                                otherwise, the one given as argument is used (must be valid)
   */
  std::shared_ptr<PolarTableBase>
  read_polar_table(const netCDF::NcVar &var, std::shared_ptr<DimensionGrid> &dimension_grid,
                   bool dimension_grid_from_var);

  std::shared_ptr<Polar> read_polar(const netCDF::NcGroup &group);

  std::shared_ptr<PolarSet> read_polar_set(const netCDF::NcGroup &group);

  std::shared_ptr<PolarNode> read_v0(const std::string &filename);

  std::shared_ptr<PolarNode> read_v1(const std::string &filename);

  std::shared_ptr<PolarNode> read_poem_nc_file(const std::string &filename);

}  // poem

#include "IO.inl"

#endif //POEM_IO_H
