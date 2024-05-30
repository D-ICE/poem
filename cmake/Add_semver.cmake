include(FetchContent)

FetchContent_Declare(semver
        GIT_REPOSITORY ${semver_URL}
        GIT_TAG ${semver_TAG}
)

set(SEMVER_OPT_BUILD_EXAMPLES OFF CACHE BOOL "")
set(SEMVER_OPT_BUILD_TESTS OFF CACHE BOOL "")
set(SEMVER_OPT_INSTALL OFF CACHE BOOL "")

message(STATUS "******* FETCHING semver dependency from ${PROJECT_NAME} (requested version: ${semver_TAG}) *******")
FetchContent_MakeAvailable(semver)
