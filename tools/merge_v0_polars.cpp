//
// Created by frongere on 28/05/24.
//

#include <iostream>
#include <filesystem>
#include <argparse/argparse.hpp>
#include <spdlog/spdlog.h>
#include <poem/poem.h>

#include <poem/thirdparty/picosha2/picosha2.h>

namespace fs = std::filesystem;
using namespace poem;


int main(int argc, char *argv[]) {

  argparse::ArgumentParser program("poem", git::GetNormalizedVersionString());

  program.add_argument("polar_files")
      .help("input polar file in NetCDF-4 format that follows the POEM file format specifications")
      .nargs(2);

  program.add_argument("--output", "-o")
      .help("NetCDF output file")
      .nargs(1);

  // Parsing command line arguments
  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error &err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    std::exit(1);
  }

  auto polar_files = program.get<std::vector<std::string>>("polar_files");
  std::vector<std::string> polar_types = {"PPP", "HVPPP", "MVPP", "VPP"};

  auto iter = polar_files.begin();
  auto type_iter = polar_types.begin();

  fs::path polar_file = *iter;
  if (!fs::exists(polar_file)) {
    spdlog::critical("Input file not found: {}", polar_file.string());
    CRITICAL_ERROR_POEM
  }
  polar_file = fs::canonical(polar_file);
  spdlog::info("Reading polar file {}", polar_file.string());

  auto reader = Reader(fs::path(*iter));
  auto perf_polar_set = reader.read();

  iter++;

  for (iter; iter!=polar_files.end(); iter++) {
    polar_file = *iter;
    if (!fs::exists(polar_file)) {
      spdlog::critical("Input file not found: {}", polar_file.string());
      CRITICAL_ERROR_POEM
    }
    polar_file = fs::canonical(polar_file);
    spdlog::info("Reading polar file {}", polar_file.string());

    reader = Reader(polar_file);
    auto pps = reader.read();
    auto ps = pps->polar_set("HPPP");
    ps->rename(*type_iter);
    ps->polar_type(*type_iter);
    perf_polar_set->AddPolarSet(ps);
    type_iter++;
  }

  fs::path output = program.get<std::string>("--output");
  perf_polar_set->to_netcdf(output);
  spdlog::info("File written: {}", output.string());

}
