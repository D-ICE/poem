add_executable(test_polar_creation test_polar_creation.cpp)
target_link_libraries(test_polar_creation poem MathUtils::MathUtils)
set_target_properties(test_polar_creation PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests)

add_executable(test_read_netcdf test_read_netcdf.cpp)
target_link_libraries(test_read_netcdf poem)
set_target_properties(test_read_netcdf PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests)
