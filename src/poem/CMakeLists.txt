add_library(poem STATIC)

target_include_directories(poem PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(poem PUBLIC
        netcdf-cxx4
        spdlog
        nlohmann_json
        )

add_subdirectory(polar)
add_subdirectory(reader)
add_subdirectory(writer)
add_subdirectory(schema)
