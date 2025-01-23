#
# RESOURCE FILES FOR TESTING
#
include(resources.cmake)

set(TESTS
        poem_tests.cpp
        test_poem.cpp
)

add_executable(poem_tests)
target_sources(poem_tests PRIVATE ${TESTS})
#target_link_libraries(poem_tests _poem MathUtils::MathUtils gtest)
target_link_libraries(poem_tests _poem gtest)
target_compile_definitions(poem_tests PUBLIC POEM_RESOURCE_DIR="${POEM_RESOURCE_DIR}")
set_target_properties(poem_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests)

add_test(poem_tests ${PROJECT_BINARY_DIR}/bin/tests/poem_tests)
