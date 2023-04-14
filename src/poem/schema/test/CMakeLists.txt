
target_compile_definitions(poem PUBLIC SCHEMA_TEST)

file(READ schema_test.json SCHEMA_TEST)
configure_file(${CMAKE_CURRENT_LIST_DIR}/schema_test.h.in ${CMAKE_CURRENT_BINARY_DIR}/schema_test.h)
target_include_directories(poem PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/../../../)
