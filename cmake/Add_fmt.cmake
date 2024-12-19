include(FetchContent)

IF (MINGW OR MSVC)
    FetchContent_Declare(fmt
            GIT_REPOSITORY ${fmt_URL}
            GIT_TAG ${fmt_TAG}
            PATCH_COMMAND git apply "${CMAKE_CURRENT_SOURCE_DIR}/cmake/patches/fmt-windows.patch"
            )
ELSE (MINGW OR MSVC)
    FetchContent_Declare(fmt
            GIT_REPOSITORY ${fmt_URL}
            GIT_TAG ${fmt_TAG}
            )
ENDIF (MINGW OR MSVC)

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/fmt)
set(BUILD_SHARED_LIBS FALSE)

set(FMT_TEST OFF CACHE BOOL "Generate the test target." FORCE)
set(FMT_DOC OFF CACHE BOOL "Generate the doc target." FORCE)
set(FMT_INSTALL OFF CACHE BOOL "Generate the install target." FORCE)

message(STATUS "******* FETCHING fmt dependency from ${PROJECT_NAME} (requested version: ${fmt_TAG}) *******")
FetchContent_MakeAvailable(fmt)

#FetchContent_GetProperties(fmt)
#if (NOT fmt_POPULATED)
#    message(STATUS "******* FETCHING fmt dependency from ${PROJECT_NAME} (requested version: ${fmt_TAG}) *******")
#    FetchContent_Populate(fmt)
#
#    # FMT BUILD OPTIONS
#    set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/fmt)
#    set(BUILD_SHARED_LIBS FALSE)
#
#    set(FMT_TEST OFF CACHE BOOL "Generate the test target." FORCE)
#    set(FMT_DOC OFF CACHE BOOL "Generate the doc target." FORCE)
#    set(FMT_INSTALL OFF CACHE BOOL "Generate the install target." FORCE)
#
#    add_subdirectory(${fmt_SOURCE_DIR} ${fmt_BINARY_DIR})
#endif ()
