#!/usr/bin/env python
#  -*- coding: utf-8 -*-

__copyright__ = "D-ICE ENGINEERING -- 2024"
__POEM_SPECS_VERSION__ = "v0"

import numpy as np
import xarray as xr

# xarray Data Model closely follows the NetCDF-4 Data Model so that we get the same concepts for Dataset, Dimensions,
# Variables, Attributes...

# POEM specifications rules are specified next to the different instructions or data creation


# Creating an empty Dataset
ds = xr.Dataset()

# Mandatory Dataset attribute (R1)
ds.attrs["polar_type"] = "ND"

# Dimensions declaration (R2)
coords = ("STW_kt", "TWS_kt", "TWA_deg", "WA_deg", "Hs_m")  # the names and order declaration comply with (R2) and (R6)

# Coordinates Variables creation into the Dataset along with unit and description attributes. Same names as Dimensions.
STW_kt = np.arange(3., 21., 1.)  # positive and strictly increasing order (R3)
ds.coords["STW_kt"] = STW_kt
ds.coords["STW_kt"].attrs = {"unit": "kt", "description": "Speed Through Water"}  # (R7)

TWS_kt = np.arange(0., 50., 5.)  # positive and strictly increasing order (R3)
ds.coords["TWS_kt"] = TWS_kt
ds.coords["TWS_kt"].attrs = {"unit": "kt", "description": "True Wind Speed"}  # (R7)

TWA_deg = np.arange(0., 180., 15.)  # positive and strictly increasing order (R3). Between 0 and 180 (R4)
ds.coords["TWA_deg"] = TWA_deg
ds.coords["TWA_deg"].attrs = {"unit": "deg", "description": "True Wind Angle"}  # (R7)

WA_deg = np.arange(0., 180., 15.)  # positive and strictly increasing order (R3). Between 0 and 180 (R4)
ds.coords["WA_deg"] = WA_deg
ds.coords["WA_deg"].attrs = {"unit": "deg", "description": "Waves Angle"}  # (R7)

Hs_m = np.arange(0., 6., 1.)
ds.coords["Hs_m"] = Hs_m
ds.coords["Hs_m"].attrs = {"unit": "m", "description": "Waves Significant Height"}  # (R7)

shape = (STW_kt.size, TWS_kt.size, TWA_deg.size, WA_deg.size, Hs_m.size)  # shape to be used for the variables 5D arrays

# Mandatory Variables (R5)
ds["BrakePower"] = (coords, np.zeros(shape))  # filled with zeros for the example
ds["BrakePower"].attrs = {"unit": "kW", "description": "Total propulsion Brake Power"}  # (R7)

ds["LEEWAY"] = (coords, np.zeros(shape))  # filled with zeros for the example
ds["LEEWAY"].attrs = {"unit": "deg", "description": "LEEWAY angle"}  # (R7)

## Writing a POEM file
ds.to_netcdf('POEM_specs_v0_minimal_example.nc')

# Note that "_FillValue" attribute is added on each Variables by xarray but is not part of POEM specifications.
