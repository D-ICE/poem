add_subdirectory(poem)
add_subdirectory(poem2)

if (${POEM_BUILD_POC})
    add_subdirectory(poc)
endif ()