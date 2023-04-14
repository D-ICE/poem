//
// Created by frongere on 11/04/23.
//

#include <filesystem>

#include "PolarReader.h"
#include "poem/exceptions.h"

#include <netcdf>
#include <spdlog/spdlog.h>
#include <map>

//#include "poem/schema/schema.h"
//#include <nlohmann/json.hpp>
#include "poem/polar/PolarSet.h"

namespace fs = std::filesystem;

namespace poem {

  std::shared_ptr<PolarSet> PolarReader::Read(const std::string &nc_polar) {

    // Does the file exist
    if (!fs::exists(nc_polar)) {
      spdlog::critical("Polar file {} NOT FOUND", nc_polar);
    }

    spdlog::info("Reading polar file {}", nc_polar);
    netCDF::NcFile dataFile(nc_polar, netCDF::NcFile::read);

    // Getting attributes
    auto atts_ = dataFile.getAtts();
    Attributes attributes;
    for (const auto &att_: atts_) {
      auto att_name = att_.first;
      std::string att_val;
      att_.second.getValues(att_val);
      attributes.add_attribute(att_name, att_val);
    }

    // The new polar set
    auto polar_set = std::make_shared<PolarSet>(attributes);

    // Get the dimensions
    auto dims_ = dataFile.getDims();
    std::map<std::string, std::shared_ptr<DimensionID>> dim_map;
    for (const auto &dim_: dims_) {
      auto dim_name = dim_.first;

      auto dim_var = dataFile.getVar(dim_name);

      std::string unit, description, min_str, max_str;

      dim_var.getAtt("unit").getValues(unit);
      dim_var.getAtt("description").getValues(description);
      dim_var.getAtt("min").getValues(min_str);
      dim_var.getAtt("max").getValues(max_str);

      double min = std::stod(min_str);
      double max = std::stod(max_str);

      auto dim_ID = std::make_shared<DimensionID>(dim_name, unit, description, min, max);
      dim_map.insert({dim_name, dim_ID});

    }


    // Get the variables
    std::map<std::string, std::shared_ptr<DimensionPointSetBase>> dim_ID_set_registry;

    auto vars_ = dataFile.getVars();
    for (const auto &var_: vars_) {

      auto var_name = var_.first;
      auto nc_var = var_.second;

      size_t dim_count = nc_var.getDimCount();

      if (dim_count == 1) {
        continue;

      } else if (dim_count == 2) {
        NIY

      } else if (dim_count == 3) {
        // TODO: faire appel a une fonction template dim_ pour eviter le boiler...

        std::string hash_name;
        for (int i = 0; i < 3; ++i) {
          hash_name += nc_var.getDim(i).getName();
        }

        std::shared_ptr<DimensionPointSet<3>> dimension_point_set;

        if (dim_ID_set_registry.find(hash_name) != dim_ID_set_registry.end()) {
          dimension_point_set = std::dynamic_pointer_cast<DimensionPointSet<3>>(dim_ID_set_registry.at(hash_name));

        } else {

          DimensionIDSet<3>::IDSet dim_ID_set;
          for (int i = 0; i < 3; ++i) {
            auto dim = nc_var.getDim(i);
            auto dim_ID = dim_map.at(dim.getName());
            dim_ID_set.at(i) = dim_ID;
          }
          auto dimension_ID_set = std::make_shared<DimensionIDSet<3>>(dim_ID_set);

          dimension_point_set = std::make_shared<DimensionPointSet<3>>(dimension_ID_set);

          for (int i = 0; i < 3; ++i) {

            auto dim = nc_var.getDim(i);
            auto dim_size = dim.getSize();
            auto dim_name = dim.getName();
            auto var = dataFile.getVar(dim_name);
            std::vector<double> values(dim_size);
            var.getVar(values.data());

            dimension_point_set->set_dimension_vector(dim_name, values);
          }

          dimension_point_set->build();

          dim_ID_set_registry.insert({hash_name, dimension_point_set});

        }

        // Now loading the variable data
        std::string unit, description;
        nc_var.getAtt("unit").getValues(unit);
        nc_var.getAtt("description").getValues(description);

        auto type = nc_var.getType();

        size_t var_size = dimension_point_set->size();

        type::POEM_TYPES poem_type;
        if (type == netCDF::ncDouble) {
          poem_type = type::DOUBLE;
          auto polar = polar_set->New<double, 3>(var_name, unit, description, poem_type, dimension_point_set);

          // Get the values of the variable
          std::vector<double> values(var_size);
          nc_var.getVar(values.data());

          size_t i = 0;
          for (const auto &dimension_point : *dimension_point_set) {
            PolarPoint<double, 3> polar_point(dimension_point, values[i]);
            polar->set_point(&polar_point);
            i++;
          }

        } else if (type == netCDF::ncInt) {
          poem_type = type::INT;
          NIY

        }

      } else if (dim_count == 4) {
        NIY

      } else if (dim_count == 5) {
        NIY

      } else if (dim_count == 6) {
        NIY

      } else {
        spdlog::critical("Polar dimensions lower than 2 or higher than 6 are forbidden");
        CRITICAL_ERROR
      }

//      STOP
    }

    return polar_set;
  }


}  // poem