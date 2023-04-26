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





  return 0;
}
