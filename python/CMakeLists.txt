include(Add_pybind11)

pybind11_add_module(pypoem pypoem.cpp)
target_link_libraries(pypoem PRIVATE _poem pybind11::headers)
add_dependencies(pypoem check_git_${PROJECT_NAME})

install(TARGETS pypoem DESTINATION pypoem)

if (POEM_BUILD_TESTS)
    set_target_properties(pypoem
            PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/tests/pypoem/pypoem
#            LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/tests/pypoem
    )
    if (NOT EXISTS "${CMAKE_SOURCE_DIR}/tests/pypoem/pypoem")
        file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/tests/pypoem/pypoem")
    endif ()
    if (NOT EXISTS "${CMAKE_SOURCE_DIR}/tests/pypoem/pypoem/__init__.py")
        file(TOUCH "${CMAKE_SOURCE_DIR}/tests/pypoem/pypoem/__init__.py")
    endif ()
endif()


