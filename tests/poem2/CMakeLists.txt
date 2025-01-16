#
# RESOURCE FILES FOR TESTING
#
include(resources.cmake)

set(TESTS
        poem2_tests.cpp
        test_poem2.cpp
)

add_executable(poem2_tests)
target_sources(poem2_tests PRIVATE ${TESTS})
target_link_libraries(poem2_tests _poem2 MathUtils::MathUtils gtest)
target_compile_definitions(poem2_tests PUBLIC POEM_RESOURCE_DIR="${POEM_RESOURCE_DIR}")
set_target_properties(poem2_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests)

add_test(poem2_tests ${PROJECT_BINARY_DIR}/bin/tests/poem2_tests)
