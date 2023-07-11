include(FetchContent)

FetchContent_Declare(spdlog
  GIT_REPOSITORY ${spdlog_URL}
  GIT_TAG ${spdlog_TAG}
)

set(SPDLOG_MASTER_PROJECT OFF CACHE BOOL "")
set(SPDLOG_FMT_EXTERNAL ON CACHE BOOL "")

message(STATUS "******* FETCHING spdlog dependency from ${PROJECT_NAME} (requested version: ${spdlog_TAG}) *******")
FetchContent_MakeAvailable(spdlog)
