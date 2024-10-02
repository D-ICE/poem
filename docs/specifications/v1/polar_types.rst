.. _polar_types_v1:

POEM Polar types (v1)
=====================

*POEM File Format Specification* format v1 introduces the capacity to store different type of polars inside one unique
*POEM File*.

The different type of polars are defined into *Groups* in the file whose name is the polar type.

5 polar types are recognized by the standard and are defined in the following:

**PPP**
    **Power Prediction, STW control, mechanical propulsion only**. *Coordinate Variables* are:

    {STW, TWS, TWA, WA, Hs}. 5D polar tables.

**HPPP**
    **Power Prediction, STW control, hybrid mechanical and wind propulsion**. *Coordinate Variables* are:

    {STW, TWS, TWA, WA, Hs}. 5D polar tables.

**HVPP**
    **Velocity Prediction, Brake Power control, hybrid mechanical and wind propulsion**. *Coordinate Variables* are:

    {PB, TWS, TWA, WA, Hs}. 5D polar tables.

**MVPP**
    **Velocity Prediction, Brake Power control, mechanical propulsion only**. *Coordinate Variables* are:

    {PB, TWS, TWA, WA, Hs}. 5D polar tables.

**VPP**
    **Velocity Prediction, No control, wind propulsion only**. *Coordinate Variables* are:

    {TWS, TWA, WA, Hs}. 4D polar tables.


with
* STW: Speed Through Water
* PB: Brake Power
* TWS: True Wind Speed
* TWA: True Wind Angle
* WA: Waves Angle
* Hs: Waves Significant Height