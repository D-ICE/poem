#
# RESOURCE FILES FOR TESTING
#
set(RESOURCE_FILES
        poem_v0_example.nc
        poem_v1_example.nc
        poem_v1_PerfPolarSet.nc 
        poem_v1_example_5_polars.nc
)

foreach (FILE ${RESOURCE_FILES})
    if (NOT EXISTS ${POEM_RESOURCE_DIR}/${FILE})
        message(STATUS "Downloading ${FILE} to ${POEM_RESOURCE_DIR}")
        execute_process(
                COMMAND
                aws s3 cp s3://dice-test-data/poem/${FILE} ${POEM_RESOURCE_DIR}/${FILE}
        )
    endif ()
    if (NOT EXISTS ${POEM_RESOURCE_DIR}/${FILE})
        message(STATUS "The Download of ${FILE} did not work properly")
    endif ()
endforeach ()

#
# TESTS # smichel: all has to be fixed, for now: commented, only test_read_poem is valid
#
set(TESTS
        test_read_poem
        test_dimension_point_set
        test_polar_set
        test_performance_polar_set
        )

foreach (test ${TESTS})
    add_executable(${test} ${test}.cpp)
    target_link_libraries(${test} _poem MathUtils::MathUtils gtest_main)
    target_compile_definitions(${test} PUBLIC TEST_SCHEMAS_DIR="${CMAKE_CURRENT_LIST_DIR}/test_schemas")
    target_compile_definitions(${test} PUBLIC POEM_RESOURCE_DIR="${POEM_RESOURCE_DIR}")
    set_target_properties(${test} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests)
    add_test(NAME ${test} COMMAND ${test})
endforeach ()
