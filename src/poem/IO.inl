//
// Created by frongere on 21/01/25.
//

namespace poem::internal {

  template<typename T>
  void write_polar_table_(std::shared_ptr<PolarTable<T>> polar_table,
                          const netCDF::NcType &nc_type,
                          netCDF::NcGroup &group) {

    auto dimension_grid = polar_table->dimension_grid();
    auto dimension_set = dimension_grid->dimension_set();

    // Storing dimensions
    size_t ndim = polar_table->dim();
    std::vector<netCDF::NcDim> dims;
    dims.reserve(ndim);

    for (size_t i = 0; i < ndim; ++i) {
      auto dimension = dimension_set->dimension(i);
      auto name = dimension->name();
      auto values = dimension_grid->values(i);

      auto dim = group.getDim(name);
      if (dim.isNull()) {
        dim = group.addDim(name, values.size());

        // The dimension as a variable
        netCDF::NcVar nc_var = group.addVar(name, netCDF::ncDouble, dim);
        nc_var.setCompression(true, true, 5);
        nc_var.putVar(values.data());

        nc_var.putAtt("unit", dimension->unit());
        nc_var.putAtt("description", dimension->description());
      }

      dims.push_back(dim);

    }

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
      spdlog::critical("Attempting to store more than one time a variable with the same name {}", polar_name);
      CRITICAL_ERROR_POEM
    }
  }

}  // poem::internal
