include(FetchContent)

FetchContent_Declare(${PROJECT_NAME}-version
  GIT_REPOSITORY ${${PROJECT_NAME}-version_URL}
  GIT_TAG ${${PROJECT_NAME}-version_TAG}
)

message(STATUS "******* FETCHING ${PROJECT_NAME}-version dependency from ${PROJECT_NAME} (requested version: ${${PROJECT_NAME}-version_TAG}) *******")
FetchContent_MakeAvailable(${PROJECT_NAME}-version)
