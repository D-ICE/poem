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
    for (const auto& dim_ : dims_) {
      auto dim_name = dim_.first;

      auto dim_var = dataFile.getVar(dim_name);

      std::string unit, description, min, max;
      dim_var.getAtt("unit").getValues(unit);
      dim_var.getAtt("description").getValues(description);
      dim_var.getAtt("min").getValues(min);
      dim_var.getAtt("max").getValues(max);




//      auto dim_group = nc_dim.getParentGroup();
//      std::string unit;
//      dim_group.getAtt("unit").getValues(unit);
//      auto group = dataFile.get


      STOP

    }


    // Get the variables
    auto vars_ = dataFile.getVars();
    for (const auto& var_ : vars_) {
      auto var_name = var_.first;
      auto nc_var = var_.second;



    }




//    // Getting dimensions
//    auto dimensions = dataFile.getDims();
//
//    std::vector<std::string> dim_names;
//    for (const auto& dim : dimensions) {
//      std::cout << dim.first << std::endl;
//      dim_names.push_back(dim.first);
//    }






    return polar_set;
  }


}  // poem