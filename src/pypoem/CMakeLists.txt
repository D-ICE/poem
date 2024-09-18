add_library(pypoem MODULE
  pypoem.cpp
)
# pybind11_add_module(pypoem
#   pypoem.cpp
# )

target_link_libraries(pypoem #PUBLIC poem)
  pybind11::module
  poem
)

# Force output directory to test folder if tests are enabled
if (${poem_enable_tests})
  set_target_properties(pypoem
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/tests/pypoem
  )
endif()

# install(TARGETS pypoem
#   LIBRARY DESTINATION .
# )
