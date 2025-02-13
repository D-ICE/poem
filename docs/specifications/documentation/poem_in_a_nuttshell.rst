.. _poem_nutshell:

POEM in a Nuttshell
===================

Why POEM?
---------

The starting point for the development of POEM was the observation of the **non-existence of an exchange standard for
vessel performance data**, particularly those with sailing assistance. As a weather routing software publisher,
D-ICE teams had to integrate performance data (generally called polar) from its customers' vessels.
Most of the time, data is stored in Excel or CSV files. This can work very well but since the performance tables are
tables of at least three dimensions, there is no commonly accepted standard for storing this type of data. Furthermore,
variables units generally do not appear in the files, the dimensions are not always given in the same order, etc.
All of this almost systematically requires rewriting a specific the conversion code, which is often error prone
and time-consuming, for everyone in the field.


What is POEM?
-------------

**POEM** represents different things at the same time:

* A **POEM Files** IS purely a *NetCDF-4* file (https://www.unidata.ucar.edu/software/netcdf/) and is fully I/O
  compatible with any library that deals with *NetCDF-4 standards*
* **POEM File Format Specification** introduces a set of rules to organise data into a *NetCDF-4* file to make a valid
  **POEM File**, understood by the **POEM Library**
* **POEM Library** is a C++ library that defines data structures to be integrated into applicative
  software such as VPP/PPP software to generate performance polar tables, visualisation
  software, weather routing solutions, embedded systems etc. It also provides fast algorithms
  to interpolate on multidimensional polar tables
* **pypoem** is a Python API to **POEM Library**
* **pypoem** is also packaged with several CLI tools to easily manipulate **POEM Files**

.. * **POEM Executable** is a command line utility to manipulate POEM files (clean, gerify version compliance etc...)

Built over NetCDF-4
~~~~~~~~~~~~~~~~~~~

**POEM Library** is built over NetCDF-4 library for reading and writing NetCDF-4 files.

Choosing NetCDF-4 as the backend file format for POEM project was motivated by the fact that NetCDF-4 offers:

* Native N-Dimensional DataSet management
* Easy sharing of NetCDF files as the library is cross-platform and exists in several programming languages (particularly C++ & Python)
* NetCDF-4 is fast. The core library is in native C
* NetCDF-4 allows to deal with very big files
* NetCDF-4 allows to keep metadata along with data, ensuring self-describing data


Why not just xarray?
~~~~~~~~~~~~~~~~~~~~

This was what we did at D-ICE before total POEM development and integration in our tools, but...

Although `xarray <https://docs.xarray.dev/en/stable/>`_ is an excellent Python library to read and write NetCDF-4 files with many
other features, xarray is a general purpose library and does not provide  special abstraction to Performance data in marine engineering.
But we could have built an abstraction layer on top of xarray.
Sure, but the second reason is that xarray is a pure Python library. As one of the goal was to interface with Weather
Routing optimization software, it was not considered efficient enough to support the million of requests done by Weather Routing
algorithms.
