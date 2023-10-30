add_library(poem STATIC
        ../pypoem/pypoem.h)

target_include_directories(poem PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(poem PUBLIC
        MathUtils::MathUtils
        poem::version
        netcdf-cxx4
        spdlog
        nlohmann_json
        dunits
        hermes
        )

set_target_properties(poem PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_subdirectory(schema)
