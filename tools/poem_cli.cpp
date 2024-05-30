//
// Created by frongere on 28/05/24.
//

#include <iostream>
#include <filesystem>
#include <argparse/argparse.hpp>
#include <spdlog/spdlog.h>
#include <poem/poem.h>

namespace fs = std::filesystem;
using namespace poem;


int main(int argc, char *argv[]) {

  argparse::ArgumentParser program("poem", version::GetNormalizedVersionString());

  program.add_argument("polar_file")
      .help("input polar file in NetCDF-4 format that follows the POEM file format specifications");

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

  SpecChecker spec_checker(polar_file);
  spec_checker.check();
//  auto performance_polar_set = read_performance_polar_set(polar_file);


  // Mettre en place la possibilite de changer le nom d'une variable



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
   * check si une polaire est au bon format poem
   * lister les PolarSet disponibles dans un fichier netCDF
   * pour un PolarSet présent, lister les variables (et les cracher dans un fichier utilisable par mship)
   *
   */





}

