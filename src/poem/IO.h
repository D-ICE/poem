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

//  namespace internal {

  template<typename T>
  void to_netcdf(std::shared_ptr<PolarTable<T>> polar_table,
                 const netCDF::NcType &nc_type,
                 netCDF::NcGroup &group);
//  }  // internal

  int current_poem_standard_version();

  std::vector<netCDF::NcDim> write_dimension_grid(std::shared_ptr<DimensionGrid> dimension_grid,
                                                  netCDF::NcGroup &group);

  /**
   * Writes a PolarTableBase into a NetCDF group (non templated)
   */
//   template <typename T>
//  void to_netcdf(std::shared_ptr<PolarTableBase> polar_table, netCDF::NcGroup &group);

  void to_netcdf(const Attributes &attributes, netCDF::NcGroup &group);

  void to_netcdf(std::shared_ptr<Polar> polar, netCDF::NcGroup &group);

  void to_netcdf(std::shared_ptr<PolarSet> polar_set, netCDF::NcGroup &group);

//  void write_polar_node(std::shared_ptr<PolarNode> polar_node, netCDF::NcGroup &group);

  void to_netcdf(std::shared_ptr<PolarNode> polar_node, netCDF::NcGroup &group);

  void to_netcdf(std::shared_ptr<PolarNode> polar_node, const std::string &filename);

//  void from_netcdf(const netCDF::NcGroup &group);


  // ===================================================================================================================
  // READERS
  // ===================================================================================================================

  int get_version(const std::string &filename);

  std::shared_ptr<DimensionGrid> read_dimension_grid_from_var(const netCDF::NcVar &var);

  /*
   * Lors de la lecture:
   *
   * le groupe root doit faire apparaitre un attribut vessel_name qui est le nom du groupe
   *
   * un PolarNode est un groupe. Tout est PolarNode finalement -> doit aiguiller la lecture ?
   *
   * on a a faire a un PolarSet si on trouve des groupes avec des noms tel que HPPP, MPPP etc...
   *
   * c'est un Polar si le nom du groupe est MPPP... on doit trouver un attribut polar_mode
   * si le groupe est root, on a plus que l'attribut polar_mode
   *
   * un PolarTable est une variable
   *
   * on veut donc les prototypes suivants:
   *
   *
   *
   * read_polar_node(group) -> ca applique les heuristiques pour determiner quel type de groupe et ca dispatche
   *
   * read_polar_set(group)
   *
   * read_polar(group)
   *
   * read_polar_table(group)
   *
   *
   *
   */


  /**
   * Read a NetCDF variable and returns it as a PolarTableBase
   *
   * @param var the NetCDF variable
   * @param dimension_grid reference to the DimensionGrid to be used
   * @param dimension_grid_from_var if true, the DimensionGrid is built from the variable,
   *                                otherwise, the one given as argument is used (must be valid)
   */
  std::shared_ptr<PolarTableBase> read_polar_table(const netCDF::NcVar &var,
                                                   std::shared_ptr<DimensionGrid> &dimension_grid,
                                                   bool dimension_grid_from_var);

  std::shared_ptr<PolarTableBase> read_polar_table(const std::string &name,
                                                   const netCDF::NcGroup &group);

  std::shared_ptr<Polar> read_polar(const netCDF::NcGroup &group, const std::string &name = "from-group");

  std::shared_ptr<PolarSet> read_polar_set(const netCDF::NcGroup &group,
                                           const std::string &polar_set_name = "from-group");

  std::shared_ptr<PolarNode> read_polar_node(const netCDF::NcGroup &group,
                                             const std::string &polar_node_name_ = "from-group");

  std::shared_ptr<PolarNode> read_v0(const std::string &filename, const std::string &root_name = "from-file");

  std::shared_ptr<PolarNode> read_v1(const std::string &filename, const std::string &root_name = "from-file");

  std::shared_ptr<PolarNode> read_poem_nc_file(const std::string &filename, const std::string &root_name = "from-file");

  POLAR_NODE_TYPE guess_polar_node_type(const netCDF::NcGroup &group);

}  // poem

#include "IO.inl"

#endif //POEM_IO_H
