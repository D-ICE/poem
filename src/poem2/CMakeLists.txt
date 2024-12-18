add_library(_poem2 INTERFACE)

target_include_directories(_poem2 INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_include_directories(_poem2 INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/..)

add_dependencies(_poem2 check_git_${PROJECT_NAME})

target_link_libraries(_poem2 INTERFACE
        Boost::boost
        MathUtils::MathUtils
        netcdf-cxx4
        spdlog
        nlohmann_json
        dunits
        semver
)

set_target_properties(_poem2 PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

if (POEM_ALLOW_DIRTY)
    target_compile_definitions(_poem2 INTERFACE ALLOW_DIRTY)
endif ()
