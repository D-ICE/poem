//
// Created by frongere on 17/05/24.
//

#ifndef POEM_PERFORMANCEPOLARSETREADER_H
#define POEM_PERFORMANCEPOLARSETREADER_H

#include <string>
#include <filesystem>
#include <spdlog/spdlog.h>

#include "exceptions.h"

namespace fs = std::filesystem;

namespace poem {

  class PerformancePolarSetReader {

   public:
    explicit PerformancePolarSetReader(const std::string &nc_polar) {
      // Does the file exist
      if (!fs::exists(nc_polar)) {
        spdlog::critical("Polar file {} NOT FOUND", nc_polar);
        CRITICAL_ERROR_POEM
      }

      NIY_POEM

    }

   private:



  };

}  // poem

#endif //POEM_PERFORMANCEPOLARSETREADER_H
