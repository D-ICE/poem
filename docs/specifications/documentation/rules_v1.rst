Rules v1
========

A *NetCDF-4 file* is a said a valid *POEM File* with version 1 if it complies with the following set of RULES

.. _rule_v1_r0:

RULE V1/R0
----------

Freedom to enrich a POEM File with any user data
    * Any Attributes for groups or Variables that are not part of the RULES **MAY** be added for user convenience
    * Any Variables that are not part of the RULES **MAY** be added to groups for user convenience
    * Any subgroup **MAY** be added to groups for user convenience


.. _rule_v1_r1:

RULE V1/R1
----------

The special ``POEM_NODE_TYPE`` attribute
    * Any NetCDF-4 Group, Coordinate Variable or Variable that own a ``POEM_NODE_TYPE`` attribute is understood as a
      POEM object and **MUST** comply with the associated POEM specification rules when it applies [#]_. We call it a
      *POEM group* in the following.
    * The parent of a POEM group **MUST** be a POEM group (except for root group)
    * The ``POEM_NODE_TYPE`` attribute **MUST** have one of the following values:

.. csv-table:: Valid values for ``POEM_NODE_TYPE`` attribute and mapping between NetCDF-4 Data Model et POEM Data Model
    :header: "``POEM_NODE_TYPE``", "NetCDF-4 object type", "POEM object type"
    :widths: 40, 60, 100

    "*POLAR_NODE*", "group", "PolarNode (and not PolarSet or Polar)"
    "*POLAR_SET*","group", "PolarSet"
    "*POLAR*", "group", "Polar"
    "*POLAR_DIMENSION*", "Coordinate Variable", "Dimension"
    "*POLAR_TABLE*", "Variable", "PolarTable"

.. note::
    The intent of the ``POEM_NODE_TYPE`` is to allow :ref:`RULE V1/R0 <rule_v1_r0>` and guarantee the robustness of the
    mapping between NetCDF-4 Data Model and POEM Data Model (see :ref:`POEM DATA MODEL <poem_data_model_header>`).

.. note::
    Although the rules of the specification are about NetCDF-4 objects (groups, Coordinate Variables or Variables),
    in the following we will talk about POEM objects instead of the NetCDF-4 counterpart, to avoid overloading the text.

    So for example, if we talk about a PolarSet, we are talking about a NetCDF-4 group with an attribute  ``POEM_NODE_TYPE``
    with the value *POLAR_SET*


.. [#] Conversely, any NetCDF-4 group, Coordinate Variable or Variable that DO NOT own the ``POEM_NODE_TYPE`` attribute
       is not read by the POEM Library


.. _rule_v1_r2:

RULE V1/R2
----------

Root Group mandatory global attributes
    * Root group **MUST** have the global attribute ``POEM_SPECIFICATION_VERSION`` with POEM File Format Specification set 
      to *1*
    * Root group **MUST** have the global attribute ``VESSEL_NAME`` filled with the vessel name
    * Root group **MUST** have the global attribute ``POEM_NODE_TYPE``. Value of this attribute depends on the context


.. _rule_v1_r3:

RULE V1/R3
----------

Required hierarchy elements
    * A PolarSet **ALWAYS** owns at least a Polar
    * A Polar **ALWAYS** owns at least a PolarTable with associated Dimensions (see also :ref:`RULE V1/R7 <rule_v1_r7>`)
    * Every of the PolarTables owned by a same Polar **MUST** have the same Dimension dependency (i.e. same DimensionSet
      in :ref:`POEM Data Model <poem_data_model>`), with the exact same order
    * A POEM File **ALWAYS** owns at least one Polar


.. _rule_v1_r4:

RULE V1/R4
----------

Polar group names and ``POEM_MODE`` attribute
    * A Polar group **MUST** own a ``POEM_MODE`` attribute with value corresponding to the :ref:`POLAR MODE <poem_polar_modes>`
      it represents
    * If a Polar group is not the root group, its name **MUST** be the same as the attribute ``POEM_MODE``
    * The case for ``POEM_MODE`` **MUST** be respected


.. _rule_v1_r5:

RULE V1/R5
----------

Variables and Coordinate Variables mandatory attibutes
    * Any Dimension or PolarTable of a Polar **MUST** have the two following attributes

      * ``unit``: specifies the unit of the Variable
      * ``description``: a one small sentence description of the variable

    * Units values set in ``unit`` attribute for Dimension and PolarTable **MUST** comply with d-units library recognized
      units


.. _rule_v1_r6:

RULE V1/R6
----------

Dimensions values
    * Dimensions values vectors **MUST** be list of positive, strictly increasing numbers
    * Angular Dimension values **MUST** be between 0 and 180 degrees
    * Dimensions values **MAY** have non-uniform value vectors

.. note::
    Currently, the only accepted Angular Dimension unit accepted is deg. This limitation could be removed in the
    future if needed


.. _rule_v1_r7:

RULE V1/R7
----------

Minimal mandatory Dimensions and PolarTables in Polar
    * The following Dimensions and PolarTables **MUST** be present in Polar, with list depending on the POLAR mode that is
      encoded in the Polar name

+----------------------+--------+--------+--------+--------+--------+----------+-----------------------------------+
|                      | MPPP   | HPPP   | MVPP   | HVPP   | VPP    |  Unit    | Description                       |
+======================+========+========+========+========+========+==========+===================================+
|         **Mandatory Dimensions**                                                                                 |
+----------------------+--------+--------+--------+--------+--------+----------+-----------------------------------+
| STW_dim              |   X    |   X    |        |        |        |   kt     | Speed Through Water dimension     |
+----------------------+--------+--------+--------+--------+--------+----------+-----------------------------------+
| Power_dim [#]_       |        |        |    X   |   X    |        |   kW     | Power dimension                   |
+----------------------+--------+--------+--------+--------+--------+----------+-----------------------------------+
| TWS_dim              |   X    |   X    |   X    |   X    |   X    |   kt     | True Wind Speed dimension         |
+----------------------+--------+--------+--------+--------+--------+----------+-----------------------------------+
| TWA_dim              |   X    |   X    |   X    |   X    |   X    |   deg    | True Wind Angle dimension         |
+----------------------+--------+--------+--------+--------+--------+----------+-----------------------------------+
| WA_dim               |   X    |   X    |   X    |   X    |   X    |   deg    | Mean Waves Angle dimension        |
+----------------------+--------+--------+--------+--------+--------+----------+-----------------------------------+
| Hs_dim               |   X    |   X    |   X    |   X    |   X    |   m      | Wave Significant Height           |
+----------------------+--------+--------+--------+--------+--------+----------+-----------------------------------+
|         **Minimal Mandatory PolarTables**                                                                        |
+----------------------+--------+--------+--------+--------+--------+----------+-----------------------------------+
| STW                  |        |        |   X    |   X    |   X    |   kt     | Speed Through Water               |
+----------------------+--------+--------+--------+--------+--------+----------+-----------------------------------+
| TOTAL_POWER [#]_     |   X    |   X    |        |        |        |   kW     | Total Power Consumption           |
+----------------------+--------+--------+--------+--------+--------+----------+-----------------------------------+
| LEEWAY               |   X    |   X    |   X    |   X    |   X    |   deg    | Leeway Angle                      |
+----------------------+--------+--------+--------+--------+--------+----------+-----------------------------------+
| SOLVER_STATUS [#]_   |   X    |   X    |   X    |   X    |   X    |    --    | Solver Status                     |
+----------------------+--------+--------+--------+--------+--------+----------+-----------------------------------+

  * Case for the Dimension and PolarTables **MUST** be respected

.. [#] Power_dim dimension is most of the time the Power associated to propulsion, i.e. BrakePower. But other convention can
       be used but it must documented.

.. [#] TOTAL_POWER PolarTable is the total consummed power onboard (propulsion power, power consummed by sails, hotel load etc...).
       What is accounted for must be documented

.. [#] SOLVER_STATUS is a special PolarTable whose data are integers. It is used to specify if a polar point comes from
       a successful computation (e.g. convergence of solver, constraints satisfied) or not.
       0 means success, i.e. it can be used. Any other value means not successful.
       Other PolarTable values corresponding to a PolarPoint for which the SOLVER_STATUS is non zero can have any
       value such as 0 or NaN.
       SOLVER_STATUS value should be used as the main source of information about quality of data.
       User can use its own non zero values to specify special cases of non-convergence of the solvers.
