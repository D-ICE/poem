//
// Created by frongere on 19/04/23.
//

#include "PolarReader.h"

namespace poem {

  std::shared_ptr<PolarSet> PolarReader::Read(const std::string &nc_polar) {
    // Does the file exist
    if (!fs::exists(nc_polar)) {
      spdlog::critical("Polar file {} NOT FOUND", nc_polar);
    }

    spdlog::info("Reading polar file {}", nc_polar);
    m_dataFile = std::make_unique<netCDF::NcFile>(nc_polar, netCDF::NcFile::read);

    // Getting attributes
    auto attributes = load_attributes();

    // FIXME: on veut pouvoir lire ND et CDA...
    Schema newest_schema = get_newest_schema();

    Schema schema(attributes.get("schema"), false); // This is the schema used to write the file, not the last one...

    // The new polar set
    m_polar_set = std::make_shared<PolarSet>();

    // Get the dimensions
    load_dimensions();

    // Get the variables
    auto vars_ = m_dataFile->getVars();
    for (const auto &var_: vars_) {
      auto var_name = var_.first;

      // If the variable corresponds to a dimension, we do not proceed
      if (m_dimension_map.find(var_name) != m_dimension_map.end()) {
        continue;
      }

      load_variable(var_name);

    }

    return m_polar_set;
  }

  const Attributes PolarReader::load_attributes() const {
    auto atts_ = m_dataFile->getAtts();
    Attributes attributes;
    for (const auto &att_: atts_) {
      auto att_name = att_.first;
      std::string att_val;
      att_.second.getValues(att_val);
      attributes.add_attribute(att_name, att_val);
    }
    return attributes;
  }

  void PolarReader::load_dimensions() {
    NIY_POEM
//    for (const auto &dim_: m_dataFile->getDims()) {
//      auto dim_name = dim_.first;
//
//      auto dim_var = m_dataFile->getVar(dim_name);
//
//      // TODO: on devrait plutot faire un load dynamique...
//      std::string unit, description, min_str, max_str;
//      dim_var.getAtt("unit").getValues(unit);
//      dim_var.getAtt("description").getValues(description);
//      dim_var.getAtt("min").getValues(min_str);
//      dim_var.getAtt("max").getValues(max_str);
//
//      double min = std::stod(min_str);
//      double max = std::stod(max_str);
//
//      auto dim_ID = std::make_shared<Dimension_>(dim_name, unit, description, min, max);
//      m_dimension_map.insert({dim_name, dim_ID});
//    }
  }

  void PolarReader::load_variable(const std::string &var_name) {
    auto nc_var = m_dataFile->getVar(var_name);
    size_t nbdim = nc_var.getDimCount();

    switch (nbdim) {
      case 1:
        return load_variable<1>(var_name);
      case 2:
        return load_variable<2>(var_name);
      case 3:
        return load_variable<3>(var_name);
      case 4:
        return load_variable<4>(var_name);
      case 5:
        return load_variable<5>(var_name);
      case 6:
        return load_variable<6>(var_name);
      default:
        spdlog::critical("Polar dimensions lower than 1 or higher than 6 are forbidden");
        CRITICAL_ERROR_POEM
    }

  }

  template<size_t _dim>
  void PolarReader::load_variable(const std::string &var_name) {
    auto nc_var = m_dataFile->getVar(var_name);
    auto type = nc_var.getType();

    switch (type.getId()) {
      case netCDF::NcType::nc_DOUBLE:
        load_variable<double, _dim>(var_name);
        return;
      case netCDF::NcType::nc_INT:
        load_variable<int, _dim>(var_name);
        return;
      default:
        spdlog::critical("Type {} is not managed yet", type.getTypeClass());
        CRITICAL_ERROR_POEM
    }
  }

  template<typename T, size_t _dim>
  void PolarReader::load_variable(const std::string &var_name) {
    NIY_POEM
//    auto nc_var = m_dataFile->getVar(var_name);
//
//    std::string hash_name;
//    for (int i = 0; i < _dim; ++i) {
//      hash_name += nc_var.getDim(i).getName();
//    }
//
//    std::shared_ptr<DimensionPointSet<_dim>> dimension_point_set;
//
//    if (m_dim_ID_set_registry.find(hash_name) != m_dim_ID_set_registry.end()) {
//      // This dimension point set is already registered, getting it from registry
//      dimension_point_set = std::dynamic_pointer_cast<DimensionPointSet<_dim>>(m_dim_ID_set_registry.at(hash_name));
//
//    } else {
//
//      typename DimensionSet<_dim>::DimensionArray dim_ID_set;
//      for (int i = 0; i < _dim; ++i) {
//        auto dim = nc_var.getDim(i);
//        auto dim_ID = m_dimension_map.at(dim.getName());
//        dim_ID_set.at(i) = dim_ID;
//      }
//      auto dimension_ID_set = std::make_shared<DimensionSet<_dim>>(dim_ID_set);
//
////      dimension_point_set = std::make_shared<DimensionPointSet<_dim>>(dimension_ID_set);
//      DimensionPointGrid<_dim> dimension_point_grid(dimension_ID_set);
//
//      for (int i = 0; i < _dim; ++i) {
//
//        auto dim = nc_var.getDim(i);
//        auto dim_size = dim.getSize();
//        auto dim_name = dim.getName();
//        auto var = m_dataFile->getVar(dim_name);
//        std::vector<double> values(dim_size);
//        var.getVar(values.data());
//
//        dimension_point_grid.set_dimension_values(dim_name, values);
////        dimension_point_set->set_dimension_values(dim_name, values);
//      }
//
//      auto dimension_point_set = dimension_point_grid.get_dimension_point_set();
////      dimension_point_set->build();
//
//      m_dim_ID_set_registry.insert({hash_name, dimension_point_set});
//
//    }
//
//    auto nc_type = nc_var.getType();
//
//    switch (nc_type.getId()) {
//      case netCDF::NcType::nc_DOUBLE:
//        return load_var_data<type::DOUBLE, _dim>(nc_var, dimension_point_set);
//      case netCDF::NcType::nc_INT:
//        return load_var_data<type::INT, _dim>(nc_var, dimension_point_set);
//      default:
//        // We should never get here
//        spdlog::critical("Type {} is not managed yet", nc_type.getTypeClass());
//        CRITICAL_ERROR
//    }

  }

  template<type::POEM_TYPES type, size_t _dim>
  void PolarReader::load_var_data(netCDF::NcVar &nc_var, std::shared_ptr<DimensionPointSet<_dim>> dimension_point_set) {

    auto var_name = nc_var.getName();

    // TODO: on devrait plutot faire un load dynamique ?
    std::string unit, description;
    nc_var.getAtt("unit").getValues(unit);
    nc_var.getAtt("description").getValues(description);

    size_t var_size = dimension_point_set->size();
    auto polar = m_polar_set->new_polar<double, _dim>(var_name, unit, description, type, dimension_point_set);

    // Get the values of the variable
    std::vector<double> values(var_size);
    nc_var.getVar(values.data());

    size_t i = 0;
    for (const auto &dimension_point: *dimension_point_set) {
      PolarPoint<double, _dim> polar_point(dimension_point, values[i]);
      polar->set_point(&polar_point);
      i++;
    }
  }

}  // poem
