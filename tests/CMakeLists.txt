add_executable(first first.cpp)
target_link_libraries(first poem)
target_compile_definitions(first PRIVATE DATA_FOLDER="${PROJECT_SOURCE_DIR}/tests/data")


add_executable(test_polar_creation test_polar_creation.cpp)
target_link_libraries(test_polar_creation poem MathUtils::MathUtils)
