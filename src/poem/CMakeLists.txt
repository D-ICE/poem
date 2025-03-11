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
        datetime

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
        Dimensional.cpp
        PolarNode.cpp
        Polar.cpp
        PolarSet.cpp
        PolarTable.cpp
        Splitter.cpp

        specifications/spec_v0.cpp
        specifications/spec_v1.cpp
)

set_target_properties(_poem PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

if (POEM_ALLOW_DIRTY)
    target_compile_definitions(_poem INTERFACE ALLOW_DIRTY)
endif ()


if (POEM_JIT)
    message(STATUS "MONITORING enabled")
    message(STATUS "JIT enabled")
    target_compile_definitions(_poem PUBLIC POEM_JIT)
    target_compile_definitions(_poem PUBLIC POEM_MON)
    target_sources(_poem PRIVATE JIT.cpp)
else ()
    message(STATUS "JIT disabled")
endif ()

if (POEM_MONITORING AND NOT POEM_JIT)
    message(STATUS "MONITORING enabled")
    target_compile_definitions(_poem PUBLIC POEM_MON)
else ()
    message(STATUS "MONITORING disabled")
endif ()
