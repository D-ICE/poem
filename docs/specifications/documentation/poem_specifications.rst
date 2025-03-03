.. _poem_specifications:

POEM Specifications
===================

POEM Library Version Scheme
---------------------------

POEM specifications follow a version number **POEM_SPECIFICATION_VERSION** that does not follow
`semantic versioning <https://semver.org/>`_ but POEM Library do.
The **POEM_SPECIFICATION_VERSION** is a unique integer that is defined to be the major number of the POEM library
version. The POEM API provides this number which is directly extracted from the library version.

Consequently, the increase of the major number of the POEM Library version necessarily means a change in the POEM
File Format Specifications.


.. _compatibility_policy:

Compatibility policy
--------------------

Policy for the compatibility between different version of the POEM File Format Specifications with respect to the POEM
Library are as follow:

READING (full reading compatibility)
    For any future version of the library, compatibility in reading must be ensured. This ensures that a POEM File written
    by an old library version must be readable by the latest version of the POEM library

WRITING (restricted writing compatibility)
    The POEM Library only ensure writing capabilities of POEM Files compliant with the **POEM_SPECIFICATION_VERSION**
    corresponding to the major number of the POEM Library version, ie the latest POEM File Format Specification.


This means that to convert any POEM File to the latest POEM File Format Specification, it is sufficient to read
the file with the lastest POEM Library and to write back the file. The generated file is then ensured to follow
the lasted standard.


Specifications
--------------

Each POEM File Format Specifications are organised as a set of RULES. A NetCDF File that is compliant with all of the rules
of a Specifications version N is said to be a POEM File.

.. note::
    The rules of the specifications apply on the POEM File, i.e. to the content of a NetCDF-4 file. They do not apply
    on the data model provided by the POEM Library. This allow to get tools to verify that a NetCDF file is compliant
    with a POEM Specification before trying to load its data into POEM data structures. Concepts used to define rules
    are those of the NetCDF-4 data model. Anyway, rules are defined to ensure correct mapping between both data models
    with a maximum of flexibility for specific extensions by users. Links to POEM data model are specified where needed.

.. toctree::
    :maxdepth: 1

    rules_v1
    rules_v0
