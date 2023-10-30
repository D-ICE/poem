cmake_minimum_required(VERSION 3.11)

project(poem)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

#-----------------------------------------------------------------------------
# Prevent in-source builds
#-----------------------------------------------------------------------------
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-source build is not possible and not recommended. Choose an empty directory for build output.")
endif (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})

#-----------------------------------------------------------------------------
# Options
#-----------------------------------------------------------------------------
option(POEM_BUILD_TESTS "Build tests" ON)
option(POEM_BUILD_TOOLS "Build tools" ON)
option(POEM_BUILD_PYBIND "Build python bindings" OFF)


cmake_policy(SET CMP0135 NEW)
cmake_policy(SET CMP0077 NEW)
cmake_policy(SET CMP0076 NEW)

#=============================================================================
# Dependencies
#=============================================================================
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/URL.conf.cmake)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

include(Add_version)

include(Add_mathutils)
include(Add_json)
include(Add_fmt)
include(Add_spdlog)
include(Add_dunits)

# netcdf related dependencies
include(Add_zlib)
include(Add_curl)
include(Add_hdf5)
include(Add_netcdf-c)
include(Add_netcdf-cxx4)


add_subdirectory(src/poem)

if (POEM_BUILD_TESTS)
    include(Add_googletest)
    enable_testing()

    # Defining a resource dir for POEM
    set(POEM_RESOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/resources)
    message(STATUS "Resources stored at ${POEM_RESOURCE_DIR}")
    file(MAKE_DIRECTORY ${POEM_RESOURCE_DIR})

    add_subdirectory(tests)
endif ()

if (POEM_BUILD_TOOLS)
    include(Add_argparse)
    add_subdirectory(tools)
endif ()

if (POEM_BUILD_PYBIND)
  add_subdirectory(src/pypoem)
endif()