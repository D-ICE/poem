include(FetchContent)

FetchContent_Declare(argparse
        GIT_REPOSITORY ${argparse_URL}
        GIT_TAG ${argparse_TAG}
        )

message(STATUS "******* FETCHING argparse dependency from ${PROJECT_NAME} (requested version: ${argparse_TAG}) *******")
FetchContent_MakeAvailable(argparse)

#FetchContent_GetProperties(argparse)
#if(NOT argparse_POPULATED)
#    message(STATUS "******* FETCHING argparse dependency from ${PROJECT_NAME} (requested version: ${argparse_TAG}) *******")
#    FetchContent_Populate(argparse)
#
#    add_subdirectory(${argparse_SOURCE_DIR} ${argparse_BINARY_DIR})
#endif()
