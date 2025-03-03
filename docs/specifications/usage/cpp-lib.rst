C++ Library
===========

The lib_poem library is the core of POEM and relies on several dependencies among them NetCDF-4 library.

Basic Usage
-----------

Building a POEM File from scratch
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. note::
    This example is correct with respect to lib_poem C++ library usage but does not produce a file that is conform
    regarding the specifications. See :ref:`poem_specifications` for more information.

.. code-block:: C++

    #include <poem/poem.h>

    using namespace poem;

    // Create the root as a PolarSet
    auto vessel = make_polar_set("vessel", "my vessel");

    // Create some Dimensions
    auto STW = make_dimension("STW", "kt", "Speed Through Water");
    auto Power = make_dimension("Power", "kW", "Power");
    auto TWS = make_dimension("TWS", "kt", "True Wind Speed");
    auto TWA = make_dimension("TWA", "kt", "True Wind Angle");

    // Create two DimensionSet (3D)
    auto dimension_set_STW_control = make_dimension_set({STW, TWS, TWA});
    auto dimension_set_Power_control = make_dimension_set({Power, TWS, TWA});

    // Create two DimensionGrid
    auto dimension_grid_STW_control = make_dimension_grid(dimension_set_STW_control);
    auto dimension_grid_Power_control = make_dimension_grid(dimension_set_Power_control);

    // Create sampling for each Dimension
    std::vector<double> STW_vector{0., 2., 4., 6., 8., 10., 12., 14.};
    std::vector<double> Power_vector{500., 1000., 1500., 2000.};
    std::vector<double> TWS_vector{0., 10., 20., 30., 40.};
    std::vector<double> TWA_vector{0., 45., 90., 135., 180.};

    // Get DimensionGrid the Dimension samplings
    dimension_grid_STW_control->set_values("STW", STW_vector);
    dimension_grid_STW_control->set_values("TWS", TWS_vector);
    dimension_grid_STW_control->set_values("TWA", TWA_vector);

    dimension_grid_Power_control->set_values("Power", Power_vector);
    dimension_grid_Power_control->set_values("TWS", TWS_vector);
    dimension_grid_Power_control->set_values("TWA", TWA_vector);

    // At this stage, we got a DimensionGrid ready and cartesian product between Dimension
    // of DimensionSet are automatically calculated to produce DimensionPoint list

    // Create a HPPP Polar and a HVPP Polar in the PolarSet
    polar_HPPP = polar_set->create_polar(HPPP, dimension_grid_STW_control);
    polar_HVPP = polar_set->create_polar(HVPP, dimension_grid_Power_control);

    // Create some PolarTable into the Polar
    polar_HPPP->create_polar_table<double>("Power", "kW", "Power polar");
    polar_HPPP->create_polar_table<int>("SomeInt"), "-", "Some Integer variable");
    polar_HVPP->create_polar_table<double>("STW", "kt", "Speed Through Water");
    polar_HVPP->create_polar_table<int>("SomeInt"), "-", "Some Integer variable");

    // Write to a POEM File
    to_netcdf(vessel, "my_vessel", "my_vessel.nc");


Loading a POEM File
~~~~~~~~~~~~~~~~~~~

.. code-block:: C++

    #include <poem/poem.h>

    using namespace poem;

    // Load the POEM File
    auto root_node = load("my_vessel.nc");

    // Print a layout of the file
    auto layout = root_node->layout();
    std::cout << layout.dump(2) << std::endl;


Building
--------

Building the lib_poem library
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To build lib_poem, you need to get the source from repository:

.. code-block:: bash

    ~$ git clone https://github.com/D-ICE/poem.git


From the root of your local git repository:

.. code-block:: bash

    ~$ mkdir build
    ~$ cd build
    ~$ cmake .. -DCMAKE_BUILD_TYPE=Release
    ~$ make _poem


Building and running the tests
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

From the root of your local git repository:

.. code-block:: bash

    ~$ mkdir build
    ~$ cd build
    ~$ cmake .. -DCMAKE_BUILD_TYPE=Release
    ~$ make poem_tests
    ~$ cd bin/tests
    ~$ ./poem_tests


Integration in cpp projects
---------------------------

To include lib_poem in your C++ project, you need to use CMake in your build chain.

The simplest way to include lib_poem is to use the FetContent module of CMake.

In your main CMakeLists.txt file:

.. code-block:: cmake

    include(FetchContent)

    FetchContent_Declare(poem
            GIT_REPOSITORY "https://github.com/D-ICE/poem.git"
            GIT_TAG "v1.6.1"  # replace with the tag you need
    )

    set(POEM_BUILD_TESTS OFF CACHE BOOL "")
    set(POEM_BUILD_TOOLS OFF CACHE BOOL "")
    set(POEM_ALLOW_DIRTY OFF CACHE BOOL "")
    set(POEM_BUILD_PYTHON OFF CACHE BOOL "")
    set(POEM_BUILD_POC OFF CACHE BOOL "")
    set(POEM_DEPS_GRAPH OFF CACHE BOOL "")

    FetchContent_MakeAvailable(poem)

    add_library(my_library)
    target_sources(my_library PUBLIC ${SOURCES})  # SOURCES variable with your .cpp implementation files
    target_link_libraries(my_library _poem)
