//
// Created by frongere on 03/04/23.
//

#ifndef POEM_JSONSCHEME_H
#define POEM_JSONSCHEME_H

#include <filesystem>

namespace fs = std::filesystem;

namespace poem {

  class JSONScheme {
   public:
    explicit JSONScheme(const fs::path &folder);


   private:
    fs::path version_to_file(int version);

   private:
    fs::path m_folder;
    int m_head_version;

  };

}  // poem

#endif //POEM_JSONSCHEME_H
