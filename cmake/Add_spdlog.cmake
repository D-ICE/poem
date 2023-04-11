include(FetchContent)

FetchContent_Declare(spdlog
  GIT_REPOSITORY ${spdlog_URL}
  GIT_TAG ${spdlog_TAG}
)

message(STATUS "******* FETCHING spdlog dependency from ${PROJECT_NAME} (requested version: ${spdlog_TAG}) *******")
FetchContent_MakeAvailable(spdlog)
