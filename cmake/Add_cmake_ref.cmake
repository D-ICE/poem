set(cmake-ref_URL "git@d-ice.gitlab.host:common/dice-tools/cmake-ref.git")
set(cmake-ref_TAG master)

include(FetchContent)

FetchContent_Declare(cmake-ref
        GIT_REPOSITORY ${cmake-ref_URL}
        GIT_TAG ${cmake-ref_TAG}
)

message(STATUS "******* FETCHING cmake-ref dependency from ${PROJECT_NAME} (requested version: ${cmake-ref_TAG}) *******")
FetchContent_MakeAvailable(cmake-ref)
