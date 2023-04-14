include(FetchContent)

FetchContent_Declare(json
        GIT_REPOSITORY ${json_URL}
        GIT_TAG ${json_TAG}
        )


set(JSON_BuildTests OFF CACHE BOOL "")
set(JSON_Install OFF CACHE BOOL "")
set(BUILD_TESTING OFF CACHE BOOL "")

message(STATUS "******* FETCHING json dependency from ${PROJECT_NAME} (requested version: ${json_TAG}) *******")
FetchContent_MakeAvailable(json)
