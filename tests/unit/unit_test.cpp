//
// Created by frongere on 07/04/23.
//

#include <gtest/gtest.h>
#include <dunits/dunits.h>

#include "poem/poem.h"

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
