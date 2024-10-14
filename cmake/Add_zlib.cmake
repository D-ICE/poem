
include(FetchContent)

FetchContent_Declare(zlib
        URL ${zlib_URL}
        )

message(STATUS "******* FETCHING zlib dependency from ${PROJECT_NAME} (requested version: ${zlib_TAG}) *******")
FetchContent_MakeAvailable(zlib)

#add_library(ZLIB::ZLIB ALIAS zlibstatic)
