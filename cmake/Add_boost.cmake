# See https://github.com/boostorg/cmake?tab=readme-ov-file#using-boost-with-fetchcontent

include(FetchContent)

FetchContent_Declare(Boost
        Boost
        URL ${boost_URL}
        DOWNLOAD_EXTRACT_TIMESTAMP ON
        EXCLUDE_FROM_ALL
)

message(STATUS "******* FINDING Boost dependency on the system from ${PROJECT_NAME} (requested version: ${boost_TAG}) *******")
FetchContent_MakeAvailable(boost)

add_library(Boost::boost INTERFACE IMPORTED)
target_link_libraries(Boost::boost INTERFACE
        Boost::headers
        Boost::multi_array
        Boost::numeric_ublas
)
