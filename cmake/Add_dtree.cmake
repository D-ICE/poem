include(FetchContent)

FetchContent_Declare(dtree
        GIT_REPOSITORY ${dtree_URL}
        GIT_TAG ${dtree_TAG}
)

message(STATUS "******* FETCHING dtree dependency from ${PROJECT_NAME} (requested version: ${dtree_TAG}) *******")
FetchContent_MakeAvailable(dtree)
