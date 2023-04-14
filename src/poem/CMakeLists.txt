add_library(poem INTERFACE)

target_include_directories(poem INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(poem INTERFACE
        netcdf-cxx4
        spdlog
        nlohmann_json
        )
