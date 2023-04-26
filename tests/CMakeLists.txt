#add_executable(test_polar_creation test_polar_creation.cpp)
#target_link_libraries(test_polar_creation poem MathUtils::MathUtils)
#set_target_properties(test_polar_creation PROPERTIES
#        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests)
#
#add_executable(test_read_netcdf test_read_netcdf.cpp)
#target_link_libraries(test_read_netcdf poem)
#set_target_properties(test_read_netcdf PROPERTIES
#        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests)

set(TESTS
#        test_polar_creation
        test_polar_compat
        )


foreach (test ${TESTS})
    add_executable(${test} ${test}.cpp)
    target_link_libraries(${test} poem MathUtils::MathUtils gtest_main)
    target_compile_definitions(${test} PUBLIC TEST_SCHEMAS_DIR="${CMAKE_CURRENT_LIST_DIR}/test_schemas")
    set_target_properties(${test} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests)

    add_test(NAME ${test}
            COMMAND ${test})
endforeach ()



