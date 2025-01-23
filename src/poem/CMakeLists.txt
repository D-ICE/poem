add_library(_poem STATIC)
# TODO: donner possibilite de faire du shared

target_include_directories(_poem PUBLIC ..)
target_include_directories(_poem PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/..)

add_dependencies(_poem check_git_${PROJECT_NAME})

target_link_libraries(_poem PUBLIC
        MathUtils::MathUtils
        netcdf-cxx4
        cools
        nlohmann_json
        dunits
        semver
        dtree

        Boost::headers
        Boost::multi_array
        Boost::numeric_ublas
)

target_sources(_poem PUBLIC
        Attributes.cpp
        Dimension.cpp
        DimensionGrid.cpp
        DimensionPoint.cpp
        DimensionSet.cpp
        enums.cpp
        IO.cpp
        Named.cpp
        PolarNode.cpp
        Polar.cpp
        PolarSet.cpp
        Splitter.cpp
)

set_target_properties(_poem PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

if (POEM_ALLOW_DIRTY)
    target_compile_definitions(_poem INTERFACE ALLOW_DIRTY)
endif ()
