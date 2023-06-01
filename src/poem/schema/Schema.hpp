
namespace poem {

  template<typename T, size_t _dim>
  void Schema::check_polar(Polar<T, _dim> *polar) const {

    auto polar_name = polar->name();

    /*
     * FIXME: Doit-on empecher de mettre des choses qui ne sont pas connues par le schema ??
     *  Doit'on mettre une option de check strict ?
     */

    // Check if polar_name is present in variables map
    if (m_variables_map.find(polar_name) == m_variables_map.end()) {

      // Looking into regexes
      bool is_known_regex = false;
      for (const auto &regex: m_regexes) {
        if (glob_match(polar_name.c_str(), regex.c_str())) {
          is_known_regex = true;
          continue;
        }
      }

      // The variable is not known
      if (!is_known_regex) {
        spdlog::critical(R"(Variable "{}" is not known by the given schema)", polar_name);
        CRITICAL_ERROR
      }

    }

    // Pas possible de check ici que les variables requises par le scheme sont toutes la
    // Il faut une methode speciale de PolarSet qui declenche le check une fois qu'on est certain
    // qu'on a identifie toutes les variables qui seront ajoutees


    // Maintentant il faut check la consistance de dimensions...

    // On veut recuperer le type et la dimension de la variable

    auto dimension_ID_set = polar->dimension_point_set()->dimension_ID_set();
    // On veut verifier que toutes les dimensions sont bien existantes
    for (size_t i = 0; i < _dim; ++i) {
//      auto dim_ID = dimension_ID_set->get(i);
      auto dim_name = dimension_ID_set->get(i)->name();

      if (m_dimensions_map.find(dim_name) == m_dimensions_map.end()) {
        spdlog::critical(R"(Variable "{}" is said to depend on dimension "{}" which is not known by the schema)",
                         polar_name, dim_name);
        CRITICAL_ERROR
      }

    }

  }


}  // poem
