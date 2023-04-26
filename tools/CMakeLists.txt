add_executable(polardump polardump.cpp)
target_link_libraries(polardump poem argparse)
set_target_properties(polardump PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
