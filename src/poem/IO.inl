//
// Created by frongere on 21/01/25.
//

namespace poem {

  template<typename T>
  void to_netcdf(std::shared_ptr<PolarTable<T>> polar_table,
                 const netCDF::NcType &nc_type,
                 netCDF::NcGroup &group) {

    auto dims = to_netcdf(polar_table->dimension_grid(), group);

    // Storing the values
    auto polar_name = polar_table->name();
    netCDF::NcVar nc_var = group.getVar(polar_name);

    if (nc_var.isNull()) {

      nc_var = group.addVar(polar_name, nc_type, dims);

      nc_var.setCompression(true, true, 5);

      nc_var.putVar(polar_table->values().data());
      nc_var.putAtt("unit", polar_table->unit());
      nc_var.putAtt("description", polar_table->description());

    } else {
      LogCriticalError("In group {}, attempting to store more than one time a variable with the same name {}",
                       group.getName(), polar_name);
      CRITICAL_ERROR_POEM
    }

  }

}  // poem
