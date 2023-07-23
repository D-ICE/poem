//
// Created by frongere on 26/04/23.
//

#include <filesystem>

#include <poem/poem.h>
#include <poem/version.h>

#include <argparse/argparse.hpp>


namespace fs = std::filesystem;
using namespace poem;

int main(int argc, char *argv[]) {

  spdlog::set_level(spdlog::level::level_enum::critical);

  argparse::ArgumentParser program("polardump", poem::version::GetNormalizedVersionString());
  program.add_argument("polar_file").help("NetCDF polar file");

  // Parsing command line arguments
  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error &err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    std::exit(1);
  }

  fs::path polar_file;
  try {
    polar_file = fs::canonical(program.get<std::string>("polar_file"));
  } catch (std::filesystem::filesystem_error &e) {
    spdlog::critical("Polar file not found: {}", std::string(polar_file));
    CRITICAL_ERROR_POEM
  }

  PolarReader reader;
  auto polar_set = reader.Read(polar_file);

  std::cout << std::endl;
  if (polar_set->is_using_newest_schema()) {
    std::cout << "Polar is using the newest schema for poem version "
              << poem::version::GetNormalizedVersionString() << std::endl;
  } else {
    std::cout << "Polar is using an older schema version " << std::endl;
  }

  std::cout << std::endl;
  std::cout << "Global attributes:" << std::endl;
  std::cout << "------------------" << std::endl;

  auto attributes = polar_set->attributes();

  for (const auto& attribute : attributes) {
    if (attribute.first == "schema") continue;

    std::cout << "*\t" << attribute.first << "\t" << attribute.second << std::endl;

  }




  return 0;
}
