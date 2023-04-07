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

  ASSERT_TRUE(polar_versions.is_this_list_consistent_with_version(variables_names_correct,
                                                                  version,
                                                                  poem::Variable::VARIABLE));
  ASSERT_FALSE(polar_versions.is_this_list_consistent_with_version(variables_names_incorrect,
                                                                  version,
                                                                  poem::Variable::VARIABLE));

//  auto v1 = polar_versions.get(version);

//  ASSERT_FALSE(v1.is_variable_recognized("v1").empty());



//  ASSERT_TRUE(polar_versions.are_variable_names_consistent(version, variables_names_correct));


}