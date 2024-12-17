add_subdirectory(poem)

if (${POEM_BUILD_POC})
    add_subdirectory(poc)
endif ()