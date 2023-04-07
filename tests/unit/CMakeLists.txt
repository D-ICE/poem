list(APPEND unit_tests
        unit_test
        )

foreach(test ${unit_tests})
    message(STATUS "   ...${test}")
    add_executable(${test} "${test}.cpp")
    target_link_libraries(${test} poem gtest gtest_main)
    add_test(NAME ${test} COMMAND ${test})
endforeach()
