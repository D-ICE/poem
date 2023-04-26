add_library(poem STATIC)

target_include_directories(poem PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(poem PUBLIC
        poem::version
        netcdf-cxx4
        spdlog
        nlohmann_json
        dunits
        Glob
        )

add_subdirectory(writer)
add_subdirectory(reader)
add_subdirectory(schema)
