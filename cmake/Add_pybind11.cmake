include(FetchContent)

FetchContent_Declare(pybind11
  GIT_REPOSITORY ${pybind11_URL}
  GIT_TAG ${pybind11_TAG}
)
FetchContent_GetProperties(pybind11)
if(NOT pybind11_POPULATED)
  message(STATUS "******* FETCHING pybind11 dependency from ${PROJECT_NAME} (requested version: ${pybind11_TAG}) *******")
  FetchContent_Populate(pybind11)

  add_subdirectory(${pybind11_SOURCE_DIR} ${pybind11_BINARY_DIR})
endif()
