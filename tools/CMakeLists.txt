#add_executable(polardump polardump.cpp)
#target_link_libraries(polardump _poem argparse)
#set_target_properties(polardump PROPERTIES
#        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(poem poem_cli.cpp)
target_link_libraries(poem _poem argparse)
set_target_properties(poem PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(poem2 poem_cli_new.cpp)
target_link_libraries(poem2 _poem argparse)
set_target_properties(poem2 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(merge_v0_polars merge_v0_polars.cpp)
target_link_libraries(merge_v0_polars _poem argparse)
set_target_properties(merge_v0_polars PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
