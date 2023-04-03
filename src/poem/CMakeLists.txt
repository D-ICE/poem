add_library(poem)

target_link_libraries(poem
        MathUtils::MathUtils
        netcdf-cxx4
        )

set_property(TARGET poem PROPERTY POSITION_INDEPENDENT_CODE ON)

target_include_directories(poem PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
        $<INSTALL_INTERFACE:include>
        )

add_subdirectory(polars)

