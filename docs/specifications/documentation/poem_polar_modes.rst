.. _poem_polar_modes:

POEM Polar Modes
================

A **Polar** can have 5 different modes, following the presence or not of mechanical propulsion, of wind assistance devices,
and the type of prediction of the performance Polar.

.. list-table:: POEM Polar Modes
    :widths: 25 25 25 25 25
    :header-rows: 1

    * - Mode
      - Prediction
      - Control
      - Motor
      - Wind Assistance
    * - MPPP
      - Power
      - Speed
      - YES
      - NO
    * - HPPP
      - Power
      - Speed
      - YES
      - YES
    * - MVPP
      - Speed
      - Power
      - YES
      - NO
    * - HVPP
      - Speed
      - Power
      - YES
      - YES
    * - VPP
      - Speed
      - None
      - NO
      - YES

Type
    The identifier of the type of Performance Polar

Prediction
    Which type of physical quantity is predicted in the performance tables

Control
    Which type of control is used, i.e. which physical quantity is used as a physical coordinate in the performance tables

Motor
    Is there a mechanical propulsion active?

Wind Propulsion
    Is there a wind propulsion active?

.. note::
    There is no PPP polar mode as it would mean that we would like to predict power to reach a prescribed speed without
    having a Motor, which is a nonsense.
