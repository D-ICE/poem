include(QueryPythonForPybind11)
find_pybind11_python_first()

pybind11_add_module(pypoem pypoem.cpp)
target_sources(pypoem PRIVATE PyWrapper.cpp)
target_link_libraries(pypoem PUBLIC
        poem
        MathUtils::MathUtils
        poem::version
        netcdf-cxx4
        spdlog
        nlohmann_json
        dunits
        hermes
        zlib
        libcurl
)

target_compile_definitions(pypoem PRIVATE VERSION_INFO=${PYPOEM_VERSION_INFO})

# Install the Python module
install(TARGETS pypoem
        COMPONENT python_modules
        DESTINATION ${PY_BUILD_CMAKE_MODULE_NAME})


# Generate stubs for the Python module
option(WITH_PY_STUBS
    "Generate Python stub files (.pyi) for the Python module." On)
if (WITH_PY_STUBS AND NOT CMAKE_CROSSCOMPILING)
    include(Pybind11Stubgen)
    pybind11_stubgen(pypoem)
    pybind11_stubgen_install(pypoem ${PY_BUILD_CMAKE_MODULE_NAME})
endif()

