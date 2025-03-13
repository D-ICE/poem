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
     public:
      explicit NodeMonitor(const std::string &nc_file) :
          nc_file(nc_file),
          nb_access(0),
          loading_time(),
          last_acces_time() {}

      inline void load() {
        loading_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        last_acces_time = loading_time;
        nb_access = 1;
      }

      inline void unload() {
        loading_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        last_acces_time = loading_time;
        nb_access = 0;
      }

      inline void new_access() {
        nb_access++;
        last_acces_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
      }

      [[nodiscard]] std::chrono::duration<int> time_since_last_access() const {
        return std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::from_time_t(last_acces_time) -
            std::chrono::system_clock::from_time_t(loading_time)
        );
      }

     private:
      fs::path nc_file;
      std::string nc_full_name;

      int nb_access = 0;

      std::time_t loading_time;
      std::time_t last_acces_time;

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
          m_unload_time(5) {}
      // END Singleton Pattern internals

      // Public methods
     public:

      void verbose(bool verbose) { m_verbose = verbose; }

      bool enabled() const { return m_enabled; }

      void enable(bool enable) { m_enabled = enable; }

      void unload_time(const int &time_seconds) {
        m_unload_time = std::chrono::duration<int>(time_seconds);
      }

      int unload_time() const {
        return m_unload_time.count();
      }

      void register_polar_table(const std::shared_ptr<PolarTableBase> &polar_table,
                                const std::string &filename,
                                const std::string &nc_full_name);

      void unregister_polar_table(const std::shared_ptr<PolarTableBase> &polar_table);

      void clear();

      void load_polar_table(const std::shared_ptr<PolarTableBase> &polar_table);

      void unload_polar_table(const std::shared_ptr<PolarTableBase> &polar_table);

      void flush();

     private:
      bool m_enabled;
      bool m_verbose;
      std::unordered_map<std::shared_ptr<PolarTableBase>, NodeMonitor> m_map;

      std::chrono::duration<int> m_unload_time;

    };

  } // jit
}  // poem

#endif //POEM_JIT
#endif //POEM_JIT_H
