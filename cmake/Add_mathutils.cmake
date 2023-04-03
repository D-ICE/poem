
include(FetchContent)

FetchContent_Declare(mathutils
        GIT_REPOSITORY ${mathutils_URL}
        GIT_TAG ${mathutils_TAG}
        )

FetchContent_GetProperties(mathutils)
if (NOT mathutils_POPULATED)
    message(STATUS "******* FETCHING mathutils dependency from ${PROJECT_NAME} (requested version: ${mathutils_TAG}) *******")
    FetchContent_Populate(mathutils)

    # MathUtils BUILD OPTIONS
    set(MATHUTILS_BUILD_TESTS OFF CACHE BOOL "")

    add_subdirectory(${mathutils_SOURCE_DIR} ${mathutils_BINARY_DIR})
endif ()
