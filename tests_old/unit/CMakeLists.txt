list(APPEND unit_tests
        unit_test
        )

set(SCHEME_PATH ${CMAKE_CURRENT_LIST_DIR}/scheme)

foreach(test ${unit_tests})
    message(STATUS "   ...${test}")
    add_executable(${test} "${test}.cpp")
    target_link_libraries(${test} poem gtest gtest_main)
    target_compile_definitions(${test} PRIVATE SCHEME_PATH="${SCHEME_PATH}")
    add_test(NAME ${test} COMMAND ${test})
endforeach()
