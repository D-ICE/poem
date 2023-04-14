include(FetchContent)

FetchContent_Declare(glob
        GIT_REPOSITORY ${glob_URL}
        GIT_TAG ${glob_TAG}
        )

message(STATUS "******* FETCHING glob dependency from ${PROJECT_NAME} (requested version: ${glob_TAG}) *******")
FetchContent_MakeAvailable(glob)
