# CMake configuration to control the look and contents of generated graphviz files.
#
# Documentation: https://cmake.org/cmake/help/latest/module/CMakeGraphVizOptions.html

#message(WARNING "config graphviz")

# TODO: devrait se trouver dans cmake-ref
# The graph name.
set(GRAPHVIZ_GRAPH_NAME "POEM dependency graph")  # default: value of CMAKE_PROJECT_NAME

# The header written at the top of the Graphviz files.
#set(GRAPHVIZ_GRAPH_HEADER "node [ fontsize = "12" ];")  # default: "node [ fontsize = "12" ];"

# The prefix for each node in the Graphviz files.
set(GRAPHVIZ_NODE_PREFIX "node")  # default: "node"

# Set to FALSE to exclude executables from the generated graphs.
set(GRAPHVIZ_EXECUTABLES TRUE)  # default: TRUE

# Set to FALSE to exclude static libraries from the generated graphs.
set(GRAPHVIZ_STATIC_LIBS TRUE)  # default: TRUE

# Set to FALSE to exclude shared libraries from the generated graphs.
set(GRAPHVIZ_SHARED_LIBS TRUE)  # default: TRUE

# Set to FALSE to exclude module libraries from the generated graphs.
set(GRAPHVIZ_MODULE_LIBS TRUE)  # default: TRUE

# Set to FALSE to exclude interface libraries from the generated graphs.
set(GRAPHVIZ_INTERFACE_LIBS TRUE)  # default: TRUE

# Set to FALSE to exclude object libraries from the generated graphs.
set(GRAPHVIZ_OBJECT_LIBS TRUE)  # default: TRUE

# Set to FALSE to exclude unknown libraries from the generated graphs.
set(GRAPHVIZ_UNKNOWN_LIBS FALSE)  # default: TRUE

# Set to FALSE to exclude external libraries from the generated graphs.
set(GRAPHVIZ_EXTERNAL_LIBS TRUE)  # default: TRUE

# Set to TRUE to include custom targets in the generated graphs.
set(GRAPHVIZ_CUSTOM_TARGETS FALSE)  # default: FALSE

# A list of regular expressions for names of targets to exclude from the generated graphs.
set(GRAPHVIZ_IGNORE_TARGETS "boost_*")  # default: empty
#set(GRAPHVIZ_IGNORE_TARGETS "")  # default: empty

# Set to FALSE to not generate per-target graphs foo.dot.<target>.
set(GRAPHVIZ_GENERATE_PER_TARGET FALSE)  # default: TRUE

# Set to FALSE to not generate depender graphs foo.dot.<target>.dependers.
set(GRAPHVIZ_GENERATE_DEPENDERS FALSE)  # default: TRUE

