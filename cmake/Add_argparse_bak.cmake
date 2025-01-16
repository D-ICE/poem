include(FetchContent)

FetchContent_Declare(argparse
        GIT_REPOSITORY ${argparse_URL}
        GIT_TAG ${argparse_TAG}
        )

message(STATUS "******* FETCHING argparse dependency from ${PROJECT_NAME} (requested version: ${argparse_TAG}) *******")
FetchContent_MakeAvailable(argparse)
