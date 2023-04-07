include(FetchContent)

FetchContent_Declare(googletest
  GIT_REPOSITORY ${googletest_URL}
  GIT_TAG ${googletest_TAG}
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

message(STATUS "******* FETCHING googletest dependency from ${PROJECT_NAME} (requested version: ${googletest_TAG}) *******")
FetchContent_MakeAvailable(googletest)
