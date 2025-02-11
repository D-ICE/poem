.. _poem_data_model_header:

POEM Data Model
===============

POEM Library is built over NetCDF-4 library and POEM Files are NetCDF-4 files. As such, POEM Library directly uses NetCDF-4
data model but provides its own data model specially adapted to vessels Performance Polar data storage and manipulation.


.. _netcdf_data_model:

NetCDF-4 Data Model
-------------------

To correctly understand POEM data model, we first need to be familiar with NetCDF-4 data model. In the following we
summarize the main concepts of the NetCDF-4 data model before introducing the POEM data model. Complete description
can be found in the
`NetCDF-4 data model documentation <https://docs.unidata.ucar.edu/nug/current/netcdf_data_set_components.html#data_model>`_.

Groups
    In NetCDF-4, data can be organized in hierarchical groups, which are analogous to directories in a filesystem. Groups
    serve as containers for variables, dimensions and attributes, as well as other groups. A NetCDF-4 file has a special group,
    called the 'root group', which is similar to the root directory in a unix filesystem.

Dimensions
    NetCDF-4 defines the sizes of all variables in terms of dimensions, so before any variables can be created the dimensions
    they use must be created first.

Variables
    NetCDF-4 variables behave much like python multidimensional array objects supplied by the numpy module.

Coordinate Variables
    It is legal for a variable to have the same name as a dimension. Such variables have no special meaning to the NetCDF
    library. However there is a convention that such variables should be treated in a special way by software using this library.

    A variable with the same name as a dimension is called a coordinate variable. It typically defines a physical
    coordinate corresponding to that dimension.

Attributes
    There are two types of attributes in a netCDF file, global and variable. Global attributes provide information about
    a group, or the entire dataset, as a whole. Variable attributes provide information about one of the variables in a group.


.. _poem_data_model:

POEM Data Model
---------------

.. todo::
    Mettre un schéma pour illustrer la hiérachie

The POEM data model has different concepts that are introduced herafter. One important point to understant is that
the POEM data model is built as a hierarchical tree structure starting at a root node. Root nodes of this tree are named
PolarNodes and it exists special PolarNodes such as PolarSet, Polar or PolarTable that provide specific meanings and
capabilities.

Dimensions
    A POEM Dimension is the name of a physical coordinate along with its unit and a textual description. It is related
    to NetCDF-4 Dimension except that it does not provides the size of this dimension. Unit and description are not part
    of NetCDF-4 Dimension but are used in POEM as attributes of the corresponding Coordinate Variable in the POEM File.

DimensionSet
    A DimensionSet is an ordered list of POEM Dimensions. It has no equivalent in NetCDF-4. It defines the coordinates of a
    multidimensional table. The size of a DimensionSet gives the dimension of the associated PolarTables. It has no direct
    equivalent in NetCDF-4 data model. The nearest concept is the Dimension dependency of NetCDF-4 Variables.

DimensionGrid
    A DimensionGrid can be seen as a realization of a DimensionSet. Each Dimension of a DimensionSet is given a value
    vector that defines the sampling values of the associated coordinate. When totally filled, a DimensionGrid is
    able to perform silently a cartesian product of every of the values vectors of the Dimensions.
    Performing this cartesian product allows to generate a flat, ordered list of PolarPoints. It has no equivalent
    in NetCDF-4 data model.

.. note::
    When performing the cartesian product of values vectors of the Dimensions of its DimensionSet, a DimensionGrid uses
    a **row major order**. This means that if we think in term of nested for loops,
    **the last dimension is moving the fastest**

PolarPoint
    A PolarPoint is a point in a DimensionGrid. It can be seen as the list of values of Dimensions somewhere in the
    DimensionGrid. Every value in a PolarTable is associated to a specific PolarPoint of the DimensionGrid. It has no
    equivalent in NetCDF-4 data model.

PolarNode
    A PolarNode is a generic node of the POEM hierarchical tree structure. It is normally associated to a special
    operational mode of a ship. An operational mode is a special configuration of a ship. For example, ballast and laden
    loading cases for a ship can be seen as different operational modes. But ballast and laden can also be divided into
    two child operational modes if we consider the usage of one or two engines.
    The concept of PolarNode is almost directly equivalent to NetCDF-4 groups, except for PolarTable that is a
    somewhat special case of PolarNode not mapped to a group bu to a Variable in NetCDF-4 (see below).

PolarSet
    A PolarSet is a special node of the POEM hierarchical tree structure. It represents a leaf Operational
    mode in the tree structure where the configuration of the ship is totally defined. A PolarSet serve as a container
    for different Polars (see below). A PolarSet is a group in a NetCDF-4 file. A PolarSet can be the root group of a
    POEM File.

Polar
    A Polar is a special node of the POEM hierarchical tree structure. In POEM, there exists 5 modes of Polar which are
    defined in :ref:`poem_polar_modes`. The POEM polar modes are the different type of performance predictions that can
    be calculated with a wind assisted vessel. A polar serves as a container for PolarTables.
    A Polar has a DimensionGrid that is shared with every PolarTables that it contains.
    A Polar is a group in the NetCDF-4 data model.

PolarTable
    A PolarTable is a special node of the POEM hierarchical tree structure. It represents a physical variable in a
    multidimensional data array. It has a unit and a description. It shares a DimensionGrid with its parent Polar and
    every of the other PolarTables in this Polar. It can contain data ot type double or int.
    Unlike Polar and PolarSet, a PolarTable is not associated to a group in the NetCDF-4 data model but to a Variable with
    mandatory Attributes (unit and description).

Attributes
    POEM data model copies its concept of attributes from NetCDF-4. Any PolarNode (PolarSet, Polar and PolarTable)
    has a set of attributes that can be defined. Attributes are a powerful mean to store metadata along with data.
