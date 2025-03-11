//
// Created by frongere on 10/03/25.
//

#include <netcdf>

#include "JIT.h"
#include "PolarNode.h"
#include "PolarTable.h"

namespace poem::jit {

  void
  poem::jit::JITManager::register_polar_table(const std::shared_ptr<PolarTableBase> node, const std::string &filename) {

    if (!fs::exists(filename)) {
      LogCriticalError("File not found: {}", filename);
      CRITICAL_ERROR_POEM
    }

    auto node_monitor = NodeMonitor(fs::absolute(filename));
//    node_monitor.nc_file = fs::absolute(filename);

    m_map.insert({node, node_monitor});

    // TODO: continuer!!
  }

  void JITManager::load_polar_table(std::shared_ptr<PolarTableBase> polar_table) {
    auto node_monitor = m_map.at(polar_table);
    if (node_monitor.is_loaded) return;

    //    auto path = polar_table->full_name();
    // TODO: construire path en retirant le premier /vessel_name
    //  Tester ce que ca fait si la Variable est directement dans le root... -> on peut juste demander a polar_table si
    //  elle a un parent, et si c'est un root...

    fs::path path = "/laden_load/laden_two_engines/VPP/STW";
    auto old_size = polar_table->memsize();

    netCDF::NcFile root_group(node_monitor.nc_file, netCDF::NcFile::read);

    // Reach the group corresponding to the Polar that hold the PolarTable
    netCDF::NcGroup group = root_group;
    for (const auto &node_name: path.parent_path()) {
      if (node_name == "/") continue;
      group = group.getGroup(node_name);
    }

    auto var = group.getVar(path.filename());
    switch (polar_table->type()) {
      case POEM_DOUBLE: {
        auto polar_table_ = polar_table->as_polar_table_double();
        polar_table_->jit_allocate();
        var.getVar(polar_table_->values().data());
      }
        break;
      case POEM_INT: {
        auto polar_table_ = polar_table->as_polar_table_int();
        polar_table_->jit_allocate();
        var.getVar(polar_table_->values().data());
      }
        break;
      default:
        LogCriticalError("[JITManager] POEM_DATATYPE not managed");
        CRITICAL_ERROR_POEM
    }
//    std::cout << "Table loaded at " << std::ctime(&node_monitor.loading_time) << std::endl;
    root_group.close();

    auto new_size = polar_table->memsize();

    node_monitor.load();

    if (m_verbose) {

      LogNormalInfo("PolarTable {} has been loaded at {}. Size was {} bytes, now it is {} bytes",
                    path.filename().string(),
                    std::ctime(&node_monitor.loading_time),
                    old_size, new_size);
    }
  }

  void JITManager::unload_polar_table(std::shared_ptr<PolarTableBase> polar_table) {
    auto old_size = polar_table->memsize();
    polar_table->jit_deallocate();
    m_map.at(polar_table).unload();
    auto new_size = polar_table->memsize();
    if (m_verbose) {
      auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
      LogNormalInfo("PolarTable {} has been unloaded at {}. Size was {} bytes, now it is {} bytes",
                    polar_table->name(),
                    std::ctime(&now),
                    old_size, new_size
                    );
    }
  }

  void JITManager::flush() {
    for (auto &node: m_map) {
      if (node.second.time_since_last_access() > m_unload_time) {
        unload_polar_table(node.first);
        node.second.unload();
      }
    }
  }

}  // poem::jit
