.. _polar_types_v1:

POEM Polar types (v1)
=====================

*POEM File Format Specification* format v1 introduces the capacity to store different type of polars inside one unique
*POEM File*.

The different type of polars are defined into *Groups* in the file whose name is the polar name (PPP, HVPP...).

The different recognized polar types are the following:

**PPP**
    **Power Prediction, motor only, STW Control**. *Coordinate Variables* are:

    {STW, TWS, TWA, WA, Hs}. 5D polar tables.

**HPPP**
    **Power Prediction, hybrid vessel, Speed Control**. *Coordinate Variables* are:

    {STW, TWS, TWA, WA, Hs}. 5D polar tables.

**HVPP-PB**
    **Velocity Prediction, hybrid vessel, Brake Power Control**. *Coordinate Variables* are:

    {PB, TWS, TWA, WA, Hs}. 5D polar tables.

**VPP-PB**
    **Velocity Prediction, motor only, Brake Power Control**. *Coordinate Variables* are:

    {PB, TWS, TWA, WA, Hs}. 5D polar tables.

**VPP**
    **Velocity Prediction, wind propulsion only, no control**. *Coordinate Variables* are:

    {TWS, TWA, WA, Hs}. 4D polar tables.
