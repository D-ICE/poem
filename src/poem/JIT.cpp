//
// Created by frongere on 10/03/25.
//

#ifdef POEM_JIT
#include <netcdf>

#include "JIT.h"
#include "PolarNode.h"
#include "PolarTable.h"

namespace poem::jit {

  void
  poem::jit::JITManager::register_polar_table(const std::shared_ptr<PolarTableBase> &polar_table,
                                              const std::string &filename,
                                              const std::string &nc_full_name) {

    if (!m_enabled) return;

    if (!fs::exists(filename)) {
      LogCriticalError("File not found: {}", filename);
      CRITICAL_ERROR_POEM
    }

    auto node_monitor = NodeMonitor(fs::absolute(filename));
    node_monitor.nc_full_name = nc_full_name;

    m_map.insert({polar_table, node_monitor});

    // TODO: continuer!!
  }

  void JITManager::unregister_polar_table(const std::shared_ptr<PolarTableBase> &polar_table) {
    if (!m_map.contains(polar_table)) return;
    m_map.erase(polar_table);
  }

  void JITManager::clear() {
    m_map.clear();
  }

  void JITManager::load_polar_table(const std::shared_ptr<PolarTableBase> &polar_table) {

    if (!m_enabled) return;
    if (polar_table->is_populated()) return;

    if (!m_map.contains(polar_table)) {
      polar_table->jit_allocate();
      return;
    }
    auto node_monitor = m_map.at(polar_table);
    node_monitor.new_access();

    fs::path path(node_monitor.nc_full_name);
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
    root_group.close();

    auto new_size = polar_table->memsize();

    node_monitor.load();

    if (m_verbose) {
      LogNormalInfo("PolarTable {} has been loaded at {}. Size was {} bytes, now it is {} bytes",
                    polar_table->full_name().string(),
                    std::ctime(&node_monitor.loading_time),
                    old_size, new_size);
    }
  }

  void JITManager::unload_polar_table(const std::shared_ptr<PolarTableBase> &polar_table) {

    if (!m_enabled) return;
    if (!polar_table->is_populated()) return;
    if (!m_map.contains(polar_table)) return;

    auto node_monitor = m_map.at(polar_table);

    auto old_size = polar_table->memsize();
    polar_table->jit_deallocate();
    node_monitor.unload();
    auto new_size = polar_table->memsize();
    if (m_verbose) {
      auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
      LogNormalInfo("PolarTable {} has been unloaded at {}. Size was {} bytes, now it is {} bytes",
                    polar_table->full_name().string(),
                    std::ctime(&now),
                    old_size, new_size
      );
    }
  }

  void JITManager::flush() {
    NIY_POEM
    if (!m_enabled) return;
    for (auto &node: m_map) {
      node.first->jit_unload();
    }
  }

}  // poem::jit

#endif //POEM_JIT