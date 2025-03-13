//
// Created by frongere on 21/01/25.
//

#ifdef POEM_JIT

#include "JIT.h"

#endif // POEM_JIT

namespace poem {

  template<typename T>
  void to_netcdf(const std::shared_ptr<PolarTable<T>> &polar_table,
                 const netCDF::NcType &nc_type,
                 const netCDF::NcGroup &group,
                 const std::string &filename) {

    auto dims = to_netcdf(polar_table->dimension_grid(), group, filename);

    // Storing the values
    auto polar_table_name = polar_table->name();

    if (group.getVars().contains(polar_table_name)) {
      LogCriticalError("In group {}, attempting to store more than one time a variable with the same name {}",
                       group.getName(), polar_table_name);
      CRITICAL_ERROR_POEM
    }

    netCDF::NcVar nc_var = group.addVar(polar_table_name, nc_type, dims);

    nc_var.setCompression(true, true, 5);

    nc_var.putVar(polar_table->values().data());
    nc_var.putAtt("unit", polar_table->unit());
    nc_var.putAtt("description", polar_table->description());
    nc_var.putAtt("POEM_NODE_TYPE", "POLAR_TABLE");

    to_netcdf(polar_table->attributes(), nc_var, filename);

    #ifdef POEM_JIT
    std::string nc_full_name =
        group.isRootGroup() ? "/" + polar_table_name : group.getName(true) + "/" + polar_table_name;
    jit::JITManager::getInstance().register_polar_table(polar_table, filename, nc_full_name);
    #endif //POEM_JIT

  }

}  // poem
