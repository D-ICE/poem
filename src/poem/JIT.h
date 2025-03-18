//
// Created by frongere on 10/03/25.
//

#ifndef POEM_JIT_H
#define POEM_JIT_H
#ifdef POEM_JIT

#include <string>
#include <filesystem>
#include <chrono>
#include <unordered_map>

#include "exceptions.h"
#include "datetime.h"


namespace fs = std::filesystem;

namespace poem {

// Forward declarations
//  class PolarNode;

  class PolarTableBase;


  namespace jit {
    // Singleton design pattern: https://stackoverflow.com/questions/1008019/how-do-you-implement-the-singleton-design-pattern

    /**
     * On va gerer le load / unload jit de maniere externe...
     * Le JIT Manager pourra avoir un index des differentes PolarTables et gerer
     * L'idee ici c'est que pour toute table de donnees, on a un tracking de:
     *  * fichier source
     *  * adresse de la table pour la retrouver
     *  * fonctions externes pour charger ou decharger la donnee
     *
     *  Ensuite on peut stocker pour chaque des metadonnees
     *  * date du chargement
     *  * date du dernier access (à la ms pres)
     *  * duree depuis le dernier access
     *  * nombre d'access
     *  *
     */

    class NodeMonitor {

      using Clock = std::chrono::system_clock;
      using Resolution = std::chrono::duration<int, std::ratio<1, 1000>>;
      using TimePoint = std::chrono::time_point<Clock, Resolution>;
      using Duration_sec = std::chrono::duration<int, std::ratio<1>>;

     public:
      NodeMonitor() {}

      explicit NodeMonitor(const std::string &nc_file) :
          m_nc_file(nc_file),
          m_nb_access(0) {}

      inline void load() {
        m_loading_time = now();
        m_last_acces_time = m_loading_time;
        m_nb_access = 0;

      }

      inline void unload() {
        m_loading_time = now();
        m_last_acces_time = m_loading_time;
        m_nb_access = 0;

      }

      inline void new_access() {
        m_nb_access++;

        m_cumulative_duration_between_access += duration_since_last_access_ms();
        m_mean_duration_between_access = m_cumulative_duration_between_access / m_nb_access;

        m_last_acces_time = now();
      }

      [[nodiscard]] Resolution duration_since_last_access_ms() const {
        return Resolution(now() - m_last_acces_time);
      }

      [[nodiscard]] Resolution duration_since_last_access_sec() const {
        return std::chrono::duration_cast<Duration_sec>(duration_since_last_access_ms());
      }

      Resolution mean_duration_between_access() const {
        return m_mean_duration_between_access;
      }

      double access_frequency_hz() const {
        return 1. / (double) std::chrono::duration_cast<Duration_sec>(m_mean_duration_between_access).count();
      }

      inline static TimePoint now() {
        return std::chrono::floor<Resolution>(Clock::now());
      }

      void log_report() const {
        LogNormalInfo(
            "[JIT] Table {}. Loading date: {}. Last access date: {}. Nb access: {}. Access frequency: {:.2f} Hz",
            m_nc_full_name,
            get_date_string(m_loading_time),
            get_date_string(m_last_acces_time),
            m_nb_access,
            access_frequency_hz());
      }

      inline static std::string get_date_string(const TimePoint &time_point) {
        auto date = Clock::to_time_t(time_point);
        return std::strtok(std::ctime(&date), "\n");
      }

     private:
      fs::path m_nc_file;
      std::string m_nc_full_name;

      unsigned int m_nb_access = 0;

      TimePoint m_loading_time;
      TimePoint m_last_acces_time;

      Resolution m_cumulative_duration_between_access;
      Resolution m_mean_duration_between_access;

      friend class JITManager;
    };


    class JITManager {

      // BEGIN Singleton Pattern internals
     public:
      static JITManager &getInstance() {
        static JITManager instance;
        return instance;
      }

      JITManager(const JITManager &) = delete;

      void operator=(const JITManager &) = delete;

     private:
      JITManager() :
          m_enabled(true),
          m_verbose(false),
          m_map(),
          m_time_threshold_sec(5) {}
      // END Singleton Pattern internals

      // Public methods
     public:

      void verbose(bool verbose) { m_verbose = verbose; }

      bool enabled() const { return m_enabled; }

      void enable(bool enable) { m_enabled = enable; }

      void duration_threshold_sec(const int &duration_sec) {
        m_time_threshold_sec = NodeMonitor::Duration_sec(duration_sec);
      }

      int time_threshold_sec() const {
        return m_time_threshold_sec.count();
      }

      bool register_polar_table(const std::shared_ptr<PolarTableBase> &polar_table,
                                const std::string &filename,
                                const std::string &nc_full_name);

      bool unregister_polar_table(const std::shared_ptr<PolarTableBase> &polar_table);

      void clear();

      bool load_polar_table(const std::shared_ptr<PolarTableBase> &polar_table);

      bool unload_polar_table(const std::shared_ptr<PolarTableBase> &polar_table);

      const NodeMonitor &get(const std::shared_ptr<PolarTableBase> &polar_table);

      bool flush(const std::shared_ptr<PolarTableBase> &polar_table);

      void flush();

     private:
      bool m_enabled;
      bool m_verbose;
      std::unordered_map<std::shared_ptr<PolarTableBase>, NodeMonitor> m_map;

      NodeMonitor::Duration_sec m_time_threshold_sec;

    };

  } // jit
}  // poem

#endif //POEM_JIT
#endif //POEM_JIT_H
