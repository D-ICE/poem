include(QueryPythonForPybind11)
find_pybind11_python_first()

pybind11_add_module(_pypoem MODULE pypoem.cpp)
pybind11_extension(_pypoem)
target_sources(_pypoem PRIVATE PyWrapper.cpp)
target_link_libraries(_pypoem PUBLIC
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

target_compile_definitions(_pypoem PRIVATE VERSION_INFO=${PYPOEM_VERSION_INFO})

message(${PY_BUILD_CMAKE_MODULE_NAME})

# Install the Python module
install(TARGETS _pypoem
        COMPONENT python_modules
        DESTINATION ${PY_BUILD_CMAKE_MODULE_NAME})


# Generate stubs for the Python module
option(WITH_PY_STUBS
    "Generate Python stub files (.pyi) for the Python module." On)
if (WITH_PY_STUBS AND NOT CMAKE_CROSSCOMPILING)
    include(Pybind11Stubgen)
    pybind11_stubgen(_pypoem)
    pybind11_stubgen_install(_pypoem ${PY_BUILD_CMAKE_MODULE_NAME})
endif()

