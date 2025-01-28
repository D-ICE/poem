include(Add_pybind11)

pybind11_add_module(pypoem pypoem.cpp)
target_link_libraries(pypoem PRIVATE _poem pybind11::headers)
#target_compile_definitions(pypoem PRIVATE VERSION_INFO=${NORMALIZED_VERSION})

install(TARGETS pypoem DESTINATION pypoem)

if (POEM_BUILD_TESTS)
    set_target_properties(pypoem
            PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/tests/pypoem
    )
endif()

