//
// Created by frongere on 28/05/24.
//

#include <iostream>
#include <filesystem>
#include <argparse/argparse.hpp>
#include <spdlog/spdlog.h>
#include <poem/poem.h>
#include "nc_file_manipulation.h"

#include <poem/thirdparty/picosha2/picosha2.h>

namespace fs = std::filesystem;
using namespace poem;


int main(int argc, char *argv[]) {

  argparse::ArgumentParser program("poem", git::GetNormalizedVersionString());

  program.add_argument("polar_file")
      .help("input polar file in NetCDF-4 format that follows the POEM file format specifications");

  program.add_argument("--info", "-i")
      .help("Get informations on the file")
      .implicit_value(true)
      .default_value(false);

  program.add_argument("--sha")
      .help("Get hash for the file (SHA256)")
      .implicit_value(true)
      .default_value(false);

  program.add_argument("--description-file", "-df")
      .help("Get an ASCII file with every coordinates")
      .implicit_value(true)
      .default_value(false);

  program.add_argument("--output", "-o")
      .help("NetCDF output file")
      .nargs(1);

  program.add_argument("--force", "-f")
      .help("Force overwriting of NetCDF file. Use with caution.")
      .implicit_value(true)
      .default_value(false);

  program.add_argument("--rename-variable", "-rv")
      .help("Rename a variable")
      .append()
      .nargs(2);

  program.add_argument("--clean-explicit", "-ce")
      .help("Takes a json file produced by the --description-file option and delete any variables that are "
            "explicitly commented in the optional_variables section. If a variable does not appear in the list, "
            "it is kept.")
      .nargs(1);

  program.add_argument("--clean-all-except", "-ca")
      .help("Takes a json file produced by the --description-file option and delete any variables that are "
            "not explicitly marked as kept. If a variable does not appear in the list, it is deleted")
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

  if (program["--sha"] == true) {
    spdlog::info("Computing SHA256 hash for file: {}", polar_file.string());
    std::ifstream f(polar_file, std::ios::binary);
    std::vector<unsigned char> s(picosha2::k_digest_size);
    picosha2::hash256(f, s.begin(), s.end());
    std::string hex_str = picosha2::bytes_to_hex_string(s);
    spdlog::info("SHA256: {}", hex_str);
    return 0;
  }


  // Check poem file format
  spdlog::info("Checking POEM file format version compliance");
  SpecRules spec_rules(get_poem_file_version(polar_file));
  if (spec_rules.check(polar_file, true)) {
    spdlog::info("File is compliant with POEM specification version: {}", spec_rules.version());
  }

  auto reader = Reader(polar_file);
  auto perf_polar_set = reader.read();

  if (program["--info"] == true) {
    get_info(polar_file);
  }

  bool force_overwrite = (program["--force"] == true);
  if (force_overwrite) {
    spdlog::warn("--force option is used. If a file exists, it will be overwritten");
  }

  /**
   * Write description file
   */

  if (program["--description-file"] == true) {
    // TODO: le force_overwrite devrait egalement etre utilise ici...
    auto description_file = polar_file.parent_path() / (polar_file.stem().string() + ".json");
    build_description_file(polar_file, description_file);
    spdlog::info("Description file written: {}", description_file.string());
  }

  bool rewrite_file = program.is_used("--output");
  /**
   * Renaming
   */

  std::unordered_map<std::string, std::string> new_varnames_map;
  if (program.is_used("--rename-variable")) {
    auto var_names = program.get<std::vector<std::string>>("--rename-variable");
    for (size_t i = 0; i < var_names.size() / 2; ++i) {
      new_varnames_map[var_names.at(2 * i)] = var_names.at(2 * i + 1);
    }
    perf_polar_set->rename_variables(new_varnames_map);
    rewrite_file = true;
  }


  /**
   * Cleaning
   */
  VariablesCleaner cleaner;
  if (program.is_used("--clean-explicit") || program.is_used("--clean-all-except")) {
    fs::path description_file;
    VariablesCleaner::MODE mode;
    if (program.is_used("--clean-explicit")) {
      description_file = program.get<std::string>("--clean-explicit");
      mode = VariablesCleaner::MODE::REMOVE_EXPLICIT;
    } else {
      description_file = program.get<std::string>("--clean-all-except");
      mode = VariablesCleaner::MODE::REMOVE_ALL_EXCEPT;
    }

    if (!fs::exists(description_file)) {
      spdlog::critical("Description file not found: {}", description_file.string());
      CRITICAL_ERROR_POEM
    }
    spdlog::info("Using the following file for cleaning: {}", description_file.string());
    description_file = fs::canonical(description_file);

    // TODO : add clean in PerformancePolarSet class
//    cleaner = VariablesCleaner(description_file, mode);
    rewrite_file = true;
  }

  /**
   * Output file
   */
  fs::path output_file;
  if (rewrite_file) {

    if (program.is_used("--output")) {
      // Does this file exist?
      output_file = program.get<std::string>("--output");
      if (fs::exists(output_file)) {
        if (force_overwrite) {
          spdlog::warn("This file is going to be overwritten: {}", output_file.string());
        } else {
          spdlog::critical("Cannot overwrite file {}", output_file.string());
          spdlog::critical("Please use --force (-f) option to overwrite (use with caution!)");
          CRITICAL_ERROR_POEM
        }
      }

      // First make a copy of polar_file
      if (output_file.is_relative()) {
        output_file = fs::weakly_canonical(fs::current_path() / output_file);
      }

    } else {
      // No output file given
      if (force_overwrite) {
        spdlog::warn("This file is going to be overwritten: {}", polar_file.string());
      } else {
        spdlog::critical("Cannot overwrite file {}", polar_file.string());
        spdlog::critical("Please use --force (-f) option or specify other file name with --output (-o) option");
        CRITICAL_ERROR_POEM
      }
      output_file = polar_file;
    }
  }


  if (rewrite_file) {
    perf_polar_set->to_netcdf(output_file);

//    // We re-write the file by applying renaming and cleaning variables
//    fs::path tmp_file = polar_file.string() + ".tmp";
//    nc_duplicate(polar_file, tmp_file, new_varnames_map, cleaner);
//    fs::rename(tmp_file, output_file);
    spdlog::info("File written: {}", output_file.string());
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
   * lister les Polar disponibles dans un fichier netCDF -> TODO
   * pour un Polar présent, lister les variables (et les cracher dans un fichier utilisable par mship) -> OK
   *
   */
  // d50893c80a02ecf22aa00fa144ab3746c506eb4557e307fe790a86c28e76e2d1

}
