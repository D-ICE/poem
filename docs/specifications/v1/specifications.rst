.. _specifications_v1:

POEM File Format Specifications (v1)
====================================

A *NetCDF-4 file* is a valid *POEM File* with version 1 if it complies with the following set of **RULES-v1**:

.. note::
    The current easiest way to manage *POEM Files* is to use Python programming language along with *xarray* module
    (while waiting for the open-source official release of the *POEM Library*)

    An example of Python script to generate a minimal mockup example of a compliant *POEM File* is available for
    download to serve as a starting point: **TODO: changer le lien, c'est une generation pour la v0 ci-dessous**
    https://drive.google.com/file/d/1u0iGnKnGnsWAC-rzynOrrV9UqNInClG-/view?usp=sharing


RULE 0 (R0)
-----------
.. _v1_r0:

* Any other *Attributes* for *Groups*, *Dimension*, *Coordinate Variables* or *Variables* that are not part of the
  following **RULES-v1** may be added for convenience
* Any other *Variables* may be added to the *Groups* defined in :ref:`(R2)<v1_r2>`.
  provided they follow **RULES** :ref:`(R7)<v1_r7>` & :ref:`(R8)<v1_r8>`

RULE 1 (R1)
-----------
.. _v1_r1:

The root *Group* MUST have the two following global *Attributes*:

* `file_type` with value poem (as a string)
* `poem_file_format_version` with value the of the *POEM File Format Version* (here v1)


RULE 2 (R2)
-----------
.. _v1_r2:

Recognized *Group* names are: PPP, HPPP, HVPP-PB, VPP-PB, VPP.
See :ref:`polar types<polar_types_v1>` for groups names definitions.

The *POEM File* must only contains groups with the preceding names.

* If **HPPP** is defined, **PPP** MUST be defined too
* If **HVPP-PB** is defined, **VPP-PB** MUST be defined too


RULE 3 (R3)
-----------
.. _v1_r3:

Every "Group" in the *POEM File* that is compliant with :ref:`(R2)<v1_r2>` MUST define its own set of *Dimensions* and
*Coordinate Variables*. Every *Group* must be self-sufficient and do not share anything with other groups although
*NetCDF-4* file format allows it.

Following the type of *Group*, the following *Dimensions* MUST be defined inside the *Groups*:

.. list-table::
    :header-rows: 1

    * - Name
      - Unit
      - Description
      - PPP
      - HPPP
      - HVPP-PB
      - VPP-PB
      - VPP
    * - STW_Coord
      - kt
      - Speed Through Water
      - X
      - X
      -
      -
      -
    * - PB_Coord
      - kW
      - Total BrakePower
      -
      -
      - X
      - X
      -
    * - TWS_Coord
      - kt
      - True Wind Speed
      - X
      - X
      - X
      - X
      - X
    * - TWA_Coord
      - deg
      - True Wind Angle
      - X
      - X
      - X
      - X
      - X
    * - WA_Coord
      - deg
      - Waves Angle
      - X
      - X
      - X
      - X
      - X
    * - Hs_Coord
      - m
      - Waves Significant Height
      - X
      - X
      - X
      - X
      - X

.. note::
    The case for dimension names MUST be respected

RULE 4 (R4)
-----------
.. _v1_r4:

*Coordinate Variables* associated with *Dimensions* defined in :ref:`(R3)<v1_r3>` MUST be of type double and be a strictly
increasing list of positive numbers

RULE 5 (R5)
-----------
.. _v1_r5:

Angular *Coordinate Variables* (i.e. TWA_Coord and WA_Coord) defined in :ref:`(R3)<v1_r3>` MUST be numbers between 0 and 180 deg.
Only symmetric polars are currently envisaged in current the specifications.

RULE 6 (R6)
-----------
.. _v1_r6:

The following Variables MUST be defined, following the type of polar considered:

.. list-table::
    :header-rows: 1

    * - Name
      - Unit
      - Description
      - PPP
      - HPPP
      - HVPP-PB
      - VPP-PB
      - VPP
    * - TotalBrakePower
      - kW
      - Total Propulsion Brake Power
      - X
      - X
      -
      -
      -
    * - STW
      - kt
      - Speed Through Water
      -
      -
      - X
      - X
      - X
    * - LEEWAY
      - deg
      - LEEWAY angle
      - X
      - X
      - X
      - X
      - X

The following *Variables* are optional but are currently understood and processed by *D-ICE ENGINEERING* *Weather
Routing Solutions* if available (any polar types):

.. list-table::
    :widths: 30 30 120
    :header-rows: 1

    * - Name
      - Unit
      - Description
    * - conso_t_h
      - t/h
      - Fuel consumption

.. note::
    * Preceding *Variable* name case MUST be respected along with the unit
    * Names for these *Variables* MIGHT change in future versions of the specifications


RULE 7 (R7)
-----------
.. _v1_r7:

Variables defined in :ref:`(R6)<v1_r6>` MUST depend on the next *Dimensions*, strictly following the order:

For **PPP** & **HPPP** (5D):
    * STW_Coord
    * TWS_Coord
    * TWA_Coord
    * WA_Coord
    * Hs_Coord

For **HVPP-PB** & **VPP-PB** (5D):
    * PB_Coord
    * TWS_Coord
    * TWA_Coord
    * WA_Coord
    * Hs_Coord

For **VPP** (4D):
    * TWS_Coord
    * TWA_Coord
    * WA_Coord
    * Hs_Coord

RULE 8 (R8)
-----------
.. _v1_r8:

*Coordinates Variables* AND *Variables* MUST be defined with at least the two following *Attributes* (case MUST be respected):

* unit
* description

This allows the *POEM Files* to be self-explanatory.
