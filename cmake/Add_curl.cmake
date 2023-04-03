
include(FetchContent)

set(SSL_ENABLED OFF CACHE BOOL "" FORCE)
set(CURL_CA_PATH "none" CACHE BOOL "" FORCE)
set(CURL_USE_SCHANNEL OFF CACHE BOOL "" FORCE)
set(CURL_USE_OPENSSL OFF CACHE BOOL "" FORCE)
set(CURL_USE_SECTRANSP OFF CACHE BOOL "" FORCE)
set(CURL_USE_MBEDTLS OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

set(CURL_ENABLE_EXPORT_TARGET ON CACHE BOOL "" FORCE)
set(CURL_ZLIB ON CACHE BOOL "" FORCE)

FetchContent_Declare(curl
        URL ${curl_URL}
        URL_HASH ${curl_URL_HASH} # the file hash for curl-7.81.0.tar.xz
        PATCH_COMMAND patch -p1 < ${PROJECT_SOURCE_DIR}/cmake/patches/${curl_PATCH}
        USES_TERMINAL_DOWNLOAD TRUE)   # <---- This is needed only for Ninja to show download progress

message(STATUS "******* FETCHING curl dependency from ${PROJECT_NAME} (requested version: ${curl_TAG}) *******")
FetchContent_MakeAvailable(curl)
