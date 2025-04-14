//
// Created by frongere on 20/02/25.
//

#include <argparse/argparse.hpp>
#include <poem/poem.h>

using namespace poem;

int main(int argc, char *argv[]) {

  argparse::ArgumentParser program("poem_downgrade_v1_to_v0", git::version_full());

  program.add_argument("intput_file").help("input POEM File");
  program.add_argument("output_file").help("output POEM File");

  // Parsing command line arguments
  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error &err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    std::exit(1);
  }

  std::string infilename = fs::canonical(fs::path(program.get<std::string>("intput_file"))).string();

  auto polar = load(infilename, true, true)->as_polar();
  auto vessel_name = polar->attributes().get("VESSEL_NAME");

  // Adding attributes that make the POEM File a v0
  std::string vessel_type;
  if (polar->mode() == MPPP) {
    vessel_type = "MOTOR";
  } else if (polar->mode() == HPPP) {
    vessel_type = "HYBRID";
  } else {
    LogCriticalError("We should not be here");
    CRITICAL_ERROR_POEM
  }
  polar->attributes().add_attribute("vessel_type", vessel_type);
  polar->attributes().add_attribute("polar_type", "ND");
  polar->attributes().add_attribute("control_variable", "STW_kt");

  // Renmaing dimensions
  polar->dimension_grid()->dimension_set()->dimension("STW_dim")->change_name("STW_kt");
  polar->dimension_grid()->dimension_set()->dimension("TWS_dim")->change_name("TWS_kt");
  polar->dimension_grid()->dimension_set()->dimension("TWA_dim")->change_name("TWA_deg");
  polar->dimension_grid()->dimension_set()->dimension("WA_dim")->change_name("WA_deg");
  polar->dimension_grid()->dimension_set()->dimension("Hs_dim")->change_name("Hs_m");

  // Renaming PolarTables (v0)
  polar->polar_table("SOLVER_STATUS")->change_name("SolverStatus");
  polar->polar_table("TOTAL_POWER")->change_name("BrakePower");

  // Changing 0 to 1 into SolverStatus PolarTable
  auto solver_status = polar->polar_table("SolverStatus");
  for (auto& value : solver_status->as_polar_table_int()->values()) {
    if (value == 0) {
      value = 1;
    }
  }

  // Writing
  std::string outfilename = fs::path(program.get<std::string>("output_file")).string();
  to_netcdf(polar, vessel_name, outfilename);

  // Reload to see if the v0 reader is happy
  load(outfilename, true, true);

  return 0;
}
