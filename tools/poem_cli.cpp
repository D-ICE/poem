//
// Created by frongere on 28/05/24.
//

#include <iostream>
#include <filesystem>
#include <argparse/argparse.hpp>
#include <spdlog/spdlog.h>
#include <poem/poem.h>

#include "nc_file_manipulation.h"

namespace fs = std::filesystem;
using namespace poem;


int main(int argc, char *argv[]) {

  argparse::ArgumentParser program("poem", version::GetNormalizedVersionString());

  program.add_argument("polar_file")
      .help("input polar file in NetCDF-4 format that follows the POEM file format specifications");

  program.add_argument("--info", "-i")
      .help("Get informations on the file")
      .implicit_value(true)
      .default_value(false);

  program.add_argument("--description-file", "-df")
      .help("Get an ASCII file with every coordinates")
      .implicit_value(true)
      .default_value(false);

  program.add_argument("--rename-variable", "-rv")
      .help("Rename a variable")
      .nargs(2);

  program.add_argument("--reduce")
      .help(
          "Takes a json file produced by the --description-file option and delete any variables that are commented in "
          "the variable vector field")
      .nargs(1);

  // Parsing command line arguments
  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error &err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    std::exit(1);
  }

  fs::path polar_file = program.get<std::string>("polar_file");
  if (!fs::exists(polar_file)) {
    spdlog::critical("Input JSON file not found: {}", polar_file.string());
    CRITICAL_ERROR_POEM
  }
  polar_file = fs::canonical(polar_file);

  spdlog::info("Reading polar file {}", polar_file.string());

  // Check poem file format
  SpecChecker checker(polar_file);
  checker.check();

  if (program["--info"] == true) {
    NIY_POEM
  }


  if (program.is_used("--rename-variable")) {
    auto var_names = program.get<std::vector<std::string>>("--rename-variable");
    rename_variable(polar_file, var_names.at(0), var_names.at(1));
  }

  if (program["--description-file"] == true) {
    std::string description_file = polar_file.parent_path() / (polar_file.stem().string() + ".json");
    get_variable_file(polar_file, description_file, checker.mandatory_variables(), checker.understood_variables());
  }




  /**
   * Fonctionnalites:
   *
   * 1 - generation d'un fichier permettant de recuperer les variables et de facilement les commenter si elles ne sont
   * pas d'interet
   *    Le but est double:
   *        * permettre au present executable poem d'elaguer un fichier existant en ne conservant que les variables
   *          presentes dans ce fichier
   *        * permettre a mship d'aller chercher les donnees qu'on veut exposer au routage
   *
   *
   * avoir des infos sur une polaire
   * check si une polaire est au bon format poem -> OK
   * lister les PolarSet disponibles dans un fichier netCDF -> TODO
   * pour un PolarSet présent, lister les variables (et les cracher dans un fichier utilisable par mship) -> OK
   *
   */


}
