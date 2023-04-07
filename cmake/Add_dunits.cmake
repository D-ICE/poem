include(FetchContent)

FetchContent_Declare(dunits
        GIT_REPOSITORY ${dunits_URL}
        GIT_TAG ${dunits_TAG}
        )

message(STATUS "******* FETCHING dunits dependency from ${PROJECT_NAME} (requested version: ${dunits_TAG}) *******")
FetchContent_MakeAvailable(dunits)
