cmake_minimum_required(VERSION 3.11)

project(poem)
set(CMAKE_CXX_STANDARD 20)

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
option(poem_enable_tests "Build tests" ON)
option(poem_build_tools "Build tools" ON)
option(poem_allow_dirty "When OFF, poem tool usage with uncommitted changes will be " OFF)
#

cmake_policy(SET CMP0135 NEW)
cmake_policy(SET CMP0077 NEW)
cmake_policy(SET CMP0076 NEW)

#=============================================================================
# Retrieving the current Git revision
#=============================================================================
set(PRE_CONFIGURE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/poem/version.h.in CACHE FILEPATH "" FORCE)
set(POST_CONFIGURE_FILE ${CMAKE_CURRENT_BINARY_DIR}/src/poem/version.h CACHE FILEPATH "" FORCE)
set(GIT_WORKING_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE FILEPATH "" FORCE)
set(GIT_STATE_FILE ${CMAKE_CURRENT_BINARY_DIR}/git-state-hash CACHE FILEPATH "" FORCE)
include(cmake/git_watcher.cmake)

if (NOT EXISTS ${POST_CONFIGURE_FILE})
    configure_file(${PRE_CONFIGURE_FILE} ${POST_CONFIGURE_FILE})
endif ()

#=============================================================================
# Dependencies
#=============================================================================
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/URL.conf.cmake)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

find_package(PythonInterp 3 REQUIRED)
find_package(PythonLibs 3 REQUIRED)

include(Add_boost)
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
include(Add_semver)

set(PYBIND11_FINDPYTHON TRUE)#ON CACHE BOOL "")
# set pybind11 dir
set(pybind11_DIR /Users/Caleb/Softwares/pybind11)
find_package(pybind11 REQUIRED)
# include(add_pybind11)

#=============================================================================
# Targets
#=============================================================================
add_subdirectory(src)

if (${poem_enable_tests})
    include(Add_googletest)
    enable_testing()

    # Defining a resource dir for POEM
    set(POEM_RESOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/resources)
    message(STATUS "Resources stored at ${POEM_RESOURCE_DIR}")
    file(MAKE_DIRECTORY ${POEM_RESOURCE_DIR})

    # include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/add_googletest.cmake)
    add_subdirectory(tests)
endif ()

# smichel: has to be fixed, for now: commented
# if (POEM_BUILD_TOOLS)
#     include(Add_argparse)
#     add_subdirectory(tools)
# endif ()
