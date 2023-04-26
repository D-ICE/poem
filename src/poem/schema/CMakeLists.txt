target_sources(poem PRIVATE
        Schema.cpp
        )

file(READ schema.json POEM_SCHEMA)
configure_file(${CMAKE_CURRENT_LIST_DIR}/schema.h.in ${CMAKE_CURRENT_BINARY_DIR}/schema.h)

# Force cmake configure is schema.json has changed
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${CMAKE_CURRENT_LIST_DIR}/schema.json)

target_include_directories(poem PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/../../)
