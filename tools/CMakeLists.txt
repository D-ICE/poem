add_executable(polardump polardump.cpp)
target_link_libraries(polardump _poem argparse)
set_target_properties(polardump PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(poem poem_cli.cpp)
target_link_libraries(poem _poem argparse)
set_target_properties(poem PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
