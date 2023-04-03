
include(FetchContent)

FetchContent_Declare(zlib
        URL ${zlib_URL}
        PATCH_COMMAND patch -p1 < ${PROJECT_SOURCE_DIR}/cmake/patches/${zlib_PATCH}
        )

message(STATUS "******* FETCHING zlib dependency from ${PROJECT_NAME} (requested version: ${zlib_TAG}) *******")
FetchContent_MakeAvailable(zlib)

#add_library(ZLIB::ZLIB ALIAS zlibstatic)
