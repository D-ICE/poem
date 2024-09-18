add_library(poem INTERFACE)

target_include_directories(poem INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_include_directories(poem INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/..)

add_dependencies(poem check_git_${PROJECT_NAME})

target_link_libraries(poem INTERFACE
        Boost::boost
        MathUtils::MathUtils
        netcdf-cxx4
        spdlog
        nlohmann_json
        dunits
        semver
)

set_target_properties(poem PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

if (poem_allow_dirty)
    target_compile_definitions(poem INTERFACE ALLOW_DIRTY)
endif ()
