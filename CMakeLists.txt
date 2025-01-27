cmake_minimum_required(VERSION 3.13)

project(poem LANGUAGES CXX)
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
option(POEM_BUILD_TESTS "Build tests" ON)
option(POEM_BUILD_TOOLS "Build tools" ON)
option(POEM_ALLOW_DIRTY "When OFF, poem tool usage with uncommitted changes will be " ON)
option(POEM_BUILD_PYTHON "Build pypoem, the python interface" ON)
option(POEM_BUILD_POC "Build proof of concept tests" ON)
#option(POEM_DEPS_GRAPH "Build the graph dependency of the lib" ON)

cmake_policy(SET CMP0135 NEW)
cmake_policy(SET CMP0077 NEW)
cmake_policy(SET CMP0076 NEW)
cmake_policy(SET CMP0167 NEW) # Boost

#=============================================================================
# Retrieving the current Git revision
#=============================================================================
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Add_cmake_ref.cmake)
list(APPEND CMAKE_MODULE_PATH ${cmake-ref_SOURCE_DIR}/cmake)

set(PRE_CONFIGURE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/poem/version.h.in CACHE FILEPATH "" FORCE)
set(POST_CONFIGURE_FILE ${CMAKE_CURRENT_BINARY_DIR}/src/poem/version.h CACHE FILEPATH "" FORCE)
set(GIT_WORKING_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE FILEPATH "" FORCE)
set(GIT_STATE_FILE ${CMAKE_CURRENT_BINARY_DIR}/git-state-hash CACHE FILEPATH "" FORCE)

if (NOT EXISTS ${POST_CONFIGURE_FILE})
    configure_file(${PRE_CONFIGURE_FILE} ${POST_CONFIGURE_FILE})
endif ()

include(git_watcher)

#=============================================================================
# Dependencies
#=============================================================================

include(Add_cools)
include(Add_Boost)
include(Add_mathutils)
include(Add_json)
include(Add_spdlog)
include(Add_dunits)
include(Add_semver)
include(Add_dtree)
include(Add_cpp-datetime)

# netcdf related dependencies
include(Add_zlib)
include(Add_curl)
include(Add_hdf5)
include(Add_netcdf-c)
include(Add_netcdf-cxx4)


#=============================================================================
# Targets
#=============================================================================
add_subdirectory(src/poem)

if (POEM_BUILD_PYTHON)
    add_subdirectory(src/pypoem)
endif ()

if (${POEM_BUILD_TESTS})
    add_subdirectory(tests)
endif ()

## smichel: has to be fixed, for now: commented
# if (POEM_BUILD_TOOLS)
#     include(Add_argparse)
#     add_subdirectory(tools)
# endif ()


