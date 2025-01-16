include(Add_googletest)
include(CTest)
enable_testing()



#add_subdirectory(poem)
add_subdirectory(poem2)

if (${POEM_BUILD_POC})
    add_subdirectory(poc)
endif ()