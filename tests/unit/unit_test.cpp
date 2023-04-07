//
// Created by frongere on 07/04/23.
//

#include <gtest/gtest.h>
#include <dunits/dunits.h>

#include "poem/poem.h"

/*
 * - pour une version de polaire donnee, pouvoir tester si une variable est reconnue.
 * - pour une version de polaire donnee, verifier que celles obligatoires figurent bien dans une liste fournie
 * - fournir un reader de polaire generique qui ne traite que les dimensions et les variables
 * - checker aussi la presence des metadonnees
 * - checker que les unites utilisees sont bien raccord avec la version
 * - checker que les descriptions utilisees sont bien raccord avec la version
 * - il faut un outil python de test de consistance de polaire (-> tools pousse sur pypi)
 *
 *
 *
 * Ou alors, on tient a jour un reader par version... -> trop relou...
 */



using namespace poem;

PolarVersions polar_versions(SCHEME_PATH);


TEST(poem, checkVersionConsistency) {

  int version = 1;
  std::vector<std::string> variables_names_correct{"v1", "v2"};
  std::vector<std::string> variables_names_incorrect{"v1", "v3"};

  // First list is the correct list
  ASSERT_TRUE(polar_versions.unknown_names(variables_names_correct,
                                           version,
                                           poem::Variable::VARIABLE).size() == 0);

  // First list is for variables, not dimensions
  ASSERT_FALSE(polar_versions.unknown_names(variables_names_correct,
                                            version,
                                            poem::Variable::DIMENSION).size() == 0);

  // Second list is not correct
  ASSERT_FALSE(polar_versions.unknown_names(variables_names_incorrect,
                                            version,
                                            poem::Variable::VARIABLE).size() == 0);

  version = 2;
  // In version 2, v2 has been modified into v2_changed
  ASSERT_FALSE(polar_versions.unknown_names(variables_names_correct,
                                           version,
                                           poem::Variable::VARIABLE).size() == 0);
  variables_names_correct[1] = "v2_changed";
  ASSERT_TRUE(polar_versions.unknown_names(variables_names_correct,
                                            version,
                                            poem::Variable::VARIABLE).size() == 0);

}


TEST(poem, testMapping) {
//  auto pair = polar_versions.map_keys(3, 1, "v2_changed", Variable::DIM_OR_VAR::VARIABLE);
//
//  int i = 0;

}