include(FetchContent)

FetchContent_Declare(pybind11
  GIT_REPOSITORY ${pybind11_URL}
  GIT_TAG ${pybind11_TAG}
)

message(STATUS "******* FETCHING pybind11 dependency from ${PROJECT_NAME} (requested version: ${pybind11_TAG}) *******")
FetchContent_MakeAvailable(pybind11)
