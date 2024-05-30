.. _specifications:

POEM File Format Specifications
===============================

A *NetCDF-4 file* is a valid *POEM File* if it complies with the following set of **RULES**:

Default RULE (R0)
-----------------

* Any other *Attributes* for *Dataset*, *Dimension*, *Coordinate Variables* or *Variables* that are not part of the
  following **RULES** may be added for convenience
* Any other *Variables* may be added to the *Dataset* provided they follow **RULES** **R6** & **R7**
* The current easiest way to manage *POEM Files* is to use Python programming language along with *xarray* module
  (while waiting for the open-source official release of the *POEM Library*)
* An example of Python script to generate a minimal mockup example of a compliant *POEM File* is available for
  download to serve as a starting point:
  https://drive.google.com/file/d/1u0iGnKnGnsWAC-rzynOrrV9UqNInClG-/view?usp=sharing


RULE 1 (R1)
-----------

The main *Dataset* MUST have an *Attribute* named polar_type with the value “ND” (string type). This is used to identify
that the *NetCDF-'* file IS a *POEM File*.

.. note::
    “ND” value is currently recognised to be not explicit and will be replaced by “POEM-<VERSION>” in future versions
    of the *POEM File Format Specifications*


RULE 2 (R2)
-----------

The following *Dimensions* MUST be defined:

.. list-table::
    :widths: 30 30 120
    :header-rows: 1

    * - Name
      - Unit
      - Description
    * - STW_kt
      - kt
      - Speed Through Water
    * - TWS_kt
      - kt
      - True Wind Speed
    * - TWA_deg
      - deg
      - True Wind Angle
    * - WA_deg
      - deg
      - Waves Angle
    * - Hs_m
      - m
      - Waves Significant Height

.. note::
    * The case for dimension names MUST be respected
    * The name for these coordinates might change in future versions of the specifications

RULE 3 (R3)
-----------

*Coordinate Variables* associated with the 5 *Dimensions* defined in **R2** MUST be of type double and be a strictly
increasing list of positive numbers

RULE 4 (R4)
-----------

Angular *Coordinate Variables* (i.e. TWA_deg and WA_deg) defined in **R2** MUST be numbers between 0 and 180 deg.
Only symmetric polars are currently envisaged in current the specifications.

RULE 5 (R5)
-----------

The following Variables MUST be defined:

.. list-table::
    :widths: 30 30 120
    :header-rows: 1

    * - Name
      - Unit
      - Description
    * - BrakePower
      - kW
      - Total Propulsion Brake Power
    * - LEEWAY
      - deg
      - LEEWAY angle

The following *Variable* is optional but is currently understood and processed by *D-ICE ENGINEERING* *Weather
Routing Solutions* if available:

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

RULE 6 (R6)
-----------

Variables defined in **R5** MUST depend on the next *Dimensions*, strictly following the order:

* STW_kt
* TWS_kt
* TWA_deg
* WA_deg
* Hs_m


RULE 7 (R7)
-----------

*Coordinates Variables* AND *Variables* MUST be defined with at least the two following *Attributes* (case MUST be respected):

* unit
* description

This allows the *POEM Files* to be self-explanatory.

