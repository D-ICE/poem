Rules v1
========

A *NetCDF-4 file* is a said a valid *POEM File* with version 1 if it complies with the following set of RULES:

RULE V1/R0
----------

Freedom to enrich the POEM File
    * Any Attributes for groups or Variables that are not part of the RULES may be added for convenience
    * Any Variables may be added to groups that are not part of the RULES may be added for convenience
    * Any subgroup may be added for convenience

RULE V1/R1
----------

Root Group mandatory global attributes
    * Root group MUST have the global attribute ``POEM_SPECIFICATION_VERSION`` with POEM File Format Specification set to 1
    * Root group MUST have the global attribute ``VESSEL_NAME`` filled with the vessel name

RULE V1/R2
----------

Groups and Variables attributes to be identified as a POEM PolarNode
    * Groups and subgroups that are part of the POEM hierarchical tree structure must have an attribute ``POEM_NODE_TYPE``
      with value to be taken in {POLAR_NODE, POLAR_SET, POLAR} following the usage intended for the group respectively as
      a PolarNode, a PolarSet or a Polar. Groups without this attribute are not read by the POEM Library.
    * Variables that are part of the POEM hierarchical tree structure must have an attribute ``POEM_NODE_TYPE`` set to the
      value POLAR_TABLE. Variables without this attribute are not read by the POEM Library.

RULE V1/R3
----------
// TODO: remplacer cette regle par le fait qu'un groupe de type POLAR DOIT avoir un nom dans MPPP, HPPP etc...
Polar groups mandatory attribute
    * Groups whose ``POEM_NODE_TYPE`` is set to POLAR MUST have an additional attribute ``POLAR_MODE`` with value to
      be taken in {MPPP, HPPP, MVPP, HVPP, VPP}. See :ref:`poem_polar_modes`.

RULE V1/R4
----------

Variables and Coordinate Variables
    * Any Variables and Coordinate Variables with attribute ``POEM_NODE_TYPE`` set to POLAR_TABLE MUST also have an
      attribute ``unit`` filled with unit of the physical quantity and ``description`` filled with a brief description of the
      physical quantity.

.. todo::
    Add a link to normalized unit from dunits


RULE V1/R5
----------

Variables in a common group // FIXME: le groupe en question doit etre de type POLAR !
    * Any Variable inside the same group and having the attribute ``POEM_NODE_TYPE`` set to POLAR_TABLE defined MUST
      have the same Dimension dependency, in the same order.

RULE V1/R6
----------

Coordinate Variables
    * Coordinate Variables having the attribute ``POEM_NODE_TYPE`` set to POLAR_TABLE MUST positive stricly increasing values.
      Regular spacing is not mandatory
    * Angular Coordinate variables (seen from their unit attribute) must have values between 0 and 180
      degrees.

.. note::
    Currently, the only accepted Angular Coordinate Variable accepted is deg. This limitation could be removed in the
    future if needed

RULE V1/R7
----------

// TODO: merger R7, R8 et R9

Speed controlled Polars dimensions
    * MPPP and HPPP Polars MUST have the following dimensions and Coordinate Variables defined, in that order: // FIXME: c'est pas la qu'on doit donner l'ordre mais dans les variables

    .. list-table::
        :widths: 30 30 120
        :header-rows: 1

        * - Name
          - Unit
          - Description
        * - STW_Coord
          - kt
          - Speed Through Water
        * - TWS_Coord
          - kt
          - True Wind Speed
        * - TWA_Coord
          - deg
          - True Wind Angle
        * - WA_Coord
          - deg
          - Waves Angle
        * - Hs_Coord
          - m
          - Waves Significant Height


Speed controlled Polars Variables
    * MPPP and HPPP Polars MUST have the following Variables defined:

    .. list-table::
        :widths: 30 30 30 120
        :header-rows: 1

        * - Name
          - Unit
          - Data Type
          - Description
        * - TOTAL_POWER
          - kW
          - double
          - Total Power
        * - LEEWAY
          - def
          - double
          - Leeway angle
        * - SOLVER_STATUS
          - -
          - int
          - Solver status

.. note::
    * The case for all names MUST be respected
    * SOLVER_STATUS is a variable to specify if the solver that generated the table converged or not. A value of 0 means
      converged whereas any other value is considered as not converged. You may adopt some numbering scheme if you need
      to track type of non convergence.

RULE V1/R8
----------

Power Controlled Polars dimensions and Variables
    * MVPP and HVPP Polars MUST have the following dimensions and Coordinate Variables defined, in that order:

    .. list-table::
        :widths: 30 30 120
        :header-rows: 1

        * - Name
          - Unit
          - Description
        * - Power_Coord
          - kW
          - Power
        * - TWS_Coord
          - kt
          - True Wind Speed
        * - TWA_Coord
          - deg
          - True Wind Angle
        * - WA_Coord
          - deg
          - Waves Angle
        * - Hs_Coord
          - m
          - Waves Significant Height

Power controlled Polars Variables
    * MVPP and HVPP Polars MUST have the following Variables defined:

    .. list-table::
        :widths: 30 30 30 120
        :header-rows: 1

        * - Name
          - Unit
          - Data Type
          - Description
        * - STW
          - kt
          - double
          - Speed Through Water
        * - LEEWAY
          - def
          - double
          - Leeway angle
        * - SOLVER_STATUS
          - -
          - int
          - Solver status

.. note::
    * The case for all names MUST be respected
    * SOLVER_STATUS is a variable to specify if the solver that generated the table converged or not. A value of 0 means
      converged whereas any other value is considered as not converged. You may adopt some numbering scheme if you need
      to track type of non convergence.

RULE V1/R9
----------

VPP (uncontrolled) Polars dimensions and Variables
    * VPP Polars MUST have the following dimensions and Coordinate Variables defined, in that order:

    .. list-table::
        :widths: 30 30 120
        :header-rows: 1

        * - Name
          - Unit
          - Description
        * - TWS_Coord
          - kt
          - True Wind Speed
        * - TWA_Coord
          - deg
          - True Wind Angle
        * - WA_Coord
          - deg
          - Waves Angle
        * - Hs_Coord
          - m
          - Waves Significant Height

VPP (uncontrolled) Polars Variables
    * VPP Polars MUST have the following Variables defined:

    .. list-table::
        :widths: 30 30 30 120
        :header-rows: 1

        * - Name
          - Unit
          - Data Type
          - Description
        * - STW
          - kt
          - double
          - Speed Through Water
        * - LEEWAY
          - def
          - double
          - Leeway angle
        * - SOLVER_STATUS
          - -
          - int
          - Solver status

.. note::
    * The case for all names MUST be respected
    * SOLVER_STATUS is a variable to specify if the solver that generated the table converged or not. A value of 0 means
      converged whereas any other value is considered as not converged. You may adopt some numbering scheme if you need
      to track type of non convergence.
