//
// Created by frongere on 11/04/23.
//

#include <filesystem>

#include "PolarReader.h"
#include "poem/exceptions.h"

#include <netcdf>
#include <spdlog/spdlog.h>

#include "poem/schema/schema.h"
#include <nlohmann/json.hpp>


namespace fs = std::filesystem;

namespace poem {

  POLAR_TYPE PolarReader::Read(const std::string &nc_polar) {

    // Does the file exist
    if (!fs::exists(nc_polar)) {
      spdlog::critical("Polar file {} NOT FOUND", nc_polar);
    }

    spdlog::info("Reading polar file {}", nc_polar);
    netCDF::NcFile datafile(nc_polar, netCDF::NcFile::read);

    // Get polar version
    try {
      datafile.getAtt("polar_version").getValues(&m_polar_version);
    } catch(netCDF::exceptions::NcBadId &e) {
      spdlog::warn("Polar version not found. Assuming version 1");
      m_polar_version = 1;
    }

    spdlog::info("Polar version {}", m_polar_version);

    // Get polar type (is that 5D_VPP, 5D_PPP, CDA?)
    std::string polar_type;
    datafile.getAtt("polar_type").getValues(polar_type);

    spdlog::info("Polar type {}", polar_type);

    std::cout << schema::schema << std::endl;





  }


}  // poem