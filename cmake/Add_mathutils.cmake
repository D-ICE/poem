include(FetchContent)

FetchContent_Declare(mathutils
        GIT_REPOSITORY ${mathutils_URL}
        GIT_TAG ${mathutils_TAG}
        )

set(MATHUTILS_BUILD_TESTS OFF CACHE BOOL "")
set(MATHUTILS_BUILD_BOOST_TESTS OFF CACHE BOOL "")

message(STATUS "******* FETCHING mathutils dependency from ${PROJECT_NAME} (requested version: ${mathutils_TAG}) *******")
FetchContent_MakeAvailable(mathutils)
