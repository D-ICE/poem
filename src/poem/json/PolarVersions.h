//
// Created by frongere on 03/04/23.
//

#ifndef POEM_POLARVERSIONS_H
#define POEM_POLARVERSIONS_H

#include <filesystem>
#include <nlohmann/json.hpp>
#include <fstream>

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace poem {

  class PolarVersion {

   public:
    explicit PolarVersion(const fs::path &file, int version) : m_version(version) {

      if (!fs::exists(file)) {
        throw std::runtime_error(std::string(file) + " not found");
      }

      std::ifstream  ifs(file);
      m_node = json::parse(ifs);

      // TODO: voir a extraire les coords et variables en speedup

    }

   private:
    int m_version;
    json m_node;

  };

  class PolarVersions {

   public:
    explicit PolarVersions(const fs::path &scheme_folder);

    int head_version() const { return m_head_version; }

    PolarVersion get(int version) const {
      return m_versions[version];
    }

   private:
    fs::path version_to_file(int version);

   private:
    fs::path m_scheme_folder;
    int m_head_version;
    std::vector<PolarVersion> m_versions;

  };

}  // poem

#endif //POEM_POLARVERSIONS_H
