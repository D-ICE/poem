pybind11_add_module(pypoem
  pypoem.cpp
)

target_link_libraries(pypoem PRIVATE _poem)

set_target_properties(pypoem
  PROPERTIES
  OUTPUT_NAME pypoem
  PREFIX "${PYTHON_MODULE_PREFIX}"
  SUFFIX "${PYTHON_MODULE_EXTENSION}"
)

# Force output directory to test folder if tests are enabled
if (${POEM_BUILD_TESTS})
  set_target_properties(pypoem
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/tests/pypoem
  )
endif()

install(TARGETS pypoem
  LIBRARY DESTINATION .
)

