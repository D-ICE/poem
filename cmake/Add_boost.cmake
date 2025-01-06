# See https://github.com/boostorg/cmake?tab=readme-ov-file#using-boost-with-fetchcontent

include(FetchContent)

FetchContent_Declare(Boost
        Boost
        URL ${boost_URL}
        DOWNLOAD_EXTRACT_TIMESTAMP ON
        EXCLUDE_FROM_ALL
)

set(BOOST_INCLUDE_LIBRARIES filesystem multi_array numeric/ublas)

message(STATUS "******* FINDING Boost dependency on the system from ${PROJECT_NAME} (requested version: ${boost_TAG}) *******")
FetchContent_MakeAvailable(boost)
