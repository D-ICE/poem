
include(FetchContent)

FetchContent_Declare(netcdf-cxx4
        URL ${netcdf-cxx4_URL}
        PATCH_COMMAND patch -p1 < ${PROJECT_SOURCE_DIR}/cmake/patches/${netcdf-cxx4_PATCH}
        )

FetchContent_GetProperties(netcdf-cxx4)
if (NOT netcdf-cxx4_POPULATED)
    message(STATUS "******* FETCHING netcdf-cxx4 dependency from ${PROJECT_NAME} (requested version: ${netcdf-cxx4_TAG}) *******")
    FetchContent_Populate(netcdf-cxx4)

    # netcdf-cxx4 BUILD OPTIONS
    set(NCXX_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    #    set(netcdf-cxx4_USE_HDF5 ON CACHE BOOL "")
    #    set(netcdf-cxx4_BUILD_TESTS OFF CACHE BOOL "")

    add_subdirectory(${netcdf-cxx4_SOURCE_DIR} ${netcdf-cxx4_BINARY_DIR})
endif ()
