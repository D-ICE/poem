
include(FetchContent)

FetchContent_Declare(hdf5
        URL ${hdf5_URL}
        #        PATCH_COMMAND patch -p1 < ${PROJECT_SOURCE_DIR}/cmake/patches/${netcdf-c_PATCH}
        )

set(BUILD_STATIC_LIBS ON CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(HDF5_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(HDF5_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(HDF5_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(HDF5_BUILD_CPP_LIB OFF CACHE BOOL "" FORCE)

set (HDF5_EXTERNALLY_CONFIGURED 1)
set (HDF5_EXTERNAL_LIB_PREFIX "")

set (HDF5_ENABLE_Z_LIB_SUPPORT ON CACHE BOOL "" FORCE)
set (HDF5_LIB_DEPENDENCIES zlibstatic)
#set (HDF5_EXPORTED_TARGETS "hdf5_hl-static hdf5-static" CACHE STRING "")
set (H5_ZLIB_HEADER "zlib.h")
#set (ZLIB_INCLUDE_DIR "${zlib_SOURCE_DIR}")
set (ZLIB_INCLUDE_DIRS "${zlib_SOURCE_DIR}")
#set(ZLIB_INCLUDE_DIR_GEN "${zlib_BINARY_DIR}")

set (ZLIB_STATIC_LIBRARY zlibstatic)
set (ZLIB_LIBRARIES zlibstatic)
#set (ZLIB_FOUND 1)

message(STATUS "******* FETCHING hdf5 dependency from ${PROJECT_NAME} (requested version: ${hdf5_TAG}) *******")
FetchContent_MakeAvailable(hdf5)

#unset(CMAKE_ARCHIVE_OUTPUT_DIRECTORY CACHE)
#unset(CMAKE_LIBRARY_OUTPUT_DIRECTORY CACHE)
#unset(CMAKE_RUNTIME_OUTPUT_DIRECTORY CACHE)

set(HDF5_C_LIBRARY hdf5-static)
set(HDF5_HL_LIBRARY hdf5_hl-static)
set(HDF5_INCLUDE_DIR
        ${hdf5_SOURCE_DIR}/src
        ${hdf5_SOURCE_DIR}/hl/src
        ${hdf5_BINARY_DIR}/src
        )

set(HDF5_VERSION ${hdf5_TAG})
