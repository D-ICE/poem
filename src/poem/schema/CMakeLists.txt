target_sources(poem PRIVATE
        SchemaChecker.cpp
        )

#file(READ schema.json POEM_SCHEMA)
#configure_file(${CMAKE_CURRENT_LIST_DIR}/schema.h.in ${CMAKE_CURRENT_BINARY_DIR}/schema.h)
#
#message(STATUS ${PROJECT_BINARY_DIR})
#message(STATUS ${CMAKE_CURRENT_BINARY_DIR}/../../)
#
#target_include_directories(poem PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/../../)

if (${POEM_BUILD_TESTS})
    add_subdirectory(test)
endif ()
