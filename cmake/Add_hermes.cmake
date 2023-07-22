
include(FetchContent)

FetchContent_Declare(hermes
        GIT_REPOSITORY ${hermes_URL}
        GIT_TAG ${hermes_TAG}
        )

FetchContent_GetProperties(hermes)
if (NOT hermes_POPULATED)
    message(STATUS "******* FETCHING hermes dependency from ${PROJECT_NAME} (requested version: ${hermes_TAG}) *******")
    FetchContent_Populate(hermes)

    # hermes BUILD OPTIONS
    set(HERMES_USE_HDF5 ON CACHE BOOL "")
    set(HERMES_BUILD_TESTS OFF CACHE BOOL "")

    add_subdirectory(${hermes_SOURCE_DIR} ${hermes_BINARY_DIR})
endif ()
