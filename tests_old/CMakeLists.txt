set(SCHEME_PATH ${PROJECT_SOURCE_DIR}/scheme)

add_executable(test_load_polar5D test_load_polar5D.cpp)
target_link_libraries(test_load_polar5D poem)

add_executable(test_json_scheme test_json_scheme.cpp)
target_link_libraries(test_json_scheme poem)
target_compile_definitions(test_json_scheme PRIVATE SCHEME_PATH="${SCHEME_PATH}")

if (POEM_BUILD_UNIT_TESTS)
    include(Add_googletest)
    enable_testing()
    add_subdirectory(unit)
endif ()