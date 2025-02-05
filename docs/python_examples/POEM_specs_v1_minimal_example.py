#!/usr/bin/env python
#  -*- coding: utf-8 -*-

__copyright__ = "D-ICE ENGINEERING -- 2024"
__POEM_SPECS_VERSION__ = "v1"

import numpy as np
import xarray as xr

# xarray Data Model closely follows the NetCDF-4 Data Model so that we get the same concepts for Dataset, Dimensions,
# Variables, Attributes...

# POEM specifications rules are specified next to the different instructions or data creation

filename = 'POEM_specs_v1_minimal_example.nc'

# Creating an empty Dataset
ds_root = xr.Dataset()

# Mandatory Dataset attribute (R1)
ds_root.attrs["file_type"] = "poem"
ds_root.attrs["poem_file_format_version"] = "v1"

# Writing global attribute to file
ds_root.to_netcdf(filename)

#
# PPP polar
#
ds_PPP = xr.Dataset()

# Dimensions declaration (R2)
coords = ("STW_Coord", "TWS_Coord", "TWA_Coord", "WA_Coord", "Hs_Coord")  # the names and order declaration comply with (R2) and (R6)

# Coordinates Variables creation into the Dataset along with unit and description attributes. Same names as Dimensions.
STW_Coord = np.arange(3., 21., 1.)  # positive and strictly increasing order (R3)
ds_PPP.coords["STW_Coord"] = STW_Coord
ds_PPP.coords["STW_Coord"].attrs = {"unit": "kt", "description": "Speed Through Water"}  # (R7)

TWS_Coord = np.arange(0., 50., 5.)  # positive and strictly increasing order (R3)
ds_PPP.coords["TWS_Coord"] = TWS_Coord
ds_PPP.coords["TWS_Coord"].attrs = {"unit": "kt", "description": "True Wind Speed"}  # (R7)

TWA_Coord = np.arange(0., 180., 15.)  # positive and strictly increasing order (R3). Between 0 and 180 (R4)
ds_PPP.coords["TWA_Coord"] = TWA_Coord
ds_PPP.coords["TWA_Coord"].attrs = {"unit": "deg", "description": "True Wind Angle"}  # (R7)

WA_Coord = np.arange(0., 180., 15.)  # positive and strictly increasing order (R3). Between 0 and 180 (R4)
ds_PPP.coords["WA_Coord"] = WA_Coord
ds_PPP.coords["WA_Coord"].attrs = {"unit": "deg", "description": "Waves Angle"}  # (R7)

Hs_Coord = np.arange(0., 6., 1.)
ds_PPP.coords["Hs_Coord"] = Hs_Coord
ds_PPP.coords["Hs_Coord"].attrs = {"unit": "m", "description": "Waves Significant Height"}  # (R7)

shape = (STW_Coord.size, TWS_Coord.size, TWA_Coord.size, WA_Coord.size, Hs_Coord.size)  # shape to be used for the variables 5D arrays

# Mandatory Variables (R5)
ds_PPP["TotalBrakePower"] = (coords, np.zeros(shape))  # filled with zeros for the example
ds_PPP["TotalBrakePower"].attrs = {"unit": "kW", "description": "Total propulsion Brake Power"}  # (R7)

ds_PPP["LEEWAY"] = (coords, np.zeros(shape))  # filled with zeros for the example
ds_PPP["LEEWAY"].attrs = {"unit": "deg", "description": "LEEWAY angle"}  # (R7)

## Writing the PPP group to file (using append mode to keep global attributes)
ds_PPP.to_netcdf(filename, group="PPP", mode='a')
ds_PPP.to_netcdf(filename, group="HPPP", mode='a')  # HPPP is the same as PPP for this example

#
# HVPP-PB polar
#

ds_HVPP = xr.Dataset()

# Dimensions declaration (R2)
coords = ("PB_Coord", "TWS_Coord", "TWA_Coord", "WA_Coord", "Hs_Coord")  # the names and order declaration comply with (R2) and (R6)

# Coordinates Variables creation into the Dataset along with unit and description attributes. Same names as Dimensions.
PB_Coord = np.arange(300., 2050, 50)  # positive and strictly increasing order (R3)
ds_HVPP.coords["PB_Coord"] = PB_Coord
ds_HVPP.coords["PB_Coord"].attrs = {"unit": "kW", "description": "Total Brake Power"}  # (R7)

TWS_Coord = np.arange(0., 50., 5.)  # positive and strictly increasing order (R3)
ds_HVPP.coords["TWS_Coord"] = TWS_Coord
ds_HVPP.coords["TWS_Coord"].attrs = {"unit": "kt", "description": "True Wind Speed"}  # (R7)

TWA_Coord = np.arange(0., 180., 15.)  # positive and strictly increasing order (R3). Between 0 and 180 (R4)
ds_HVPP.coords["TWA_Coord"] = TWA_Coord
ds_HVPP.coords["TWA_Coord"].attrs = {"unit": "deg", "description": "True Wind Angle"}  # (R7)

WA_Coord = np.arange(0., 180., 15.)  # positive and strictly increasing order (R3). Between 0 and 180 (R4)
ds_HVPP.coords["WA_Coord"] = WA_Coord
ds_HVPP.coords["WA_Coord"].attrs = {"unit": "deg", "description": "Waves Angle"}  # (R7)

Hs_Coord = np.arange(0., 6., 1.)
ds_HVPP.coords["Hs_Coord"] = Hs_Coord
ds_HVPP.coords["Hs_Coord"].attrs = {"unit": "m", "description": "Waves Significant Height"}  # (R7)

shape = (PB_Coord.size, TWS_Coord.size, TWA_Coord.size, WA_Coord.size, Hs_Coord.size)  # shape to be used for the variables 5D arrays

# Mandatory Variables (R5)
ds_HVPP["STW"] = (coords, np.zeros(shape))  # filled with zeros for the example
ds_HVPP["STW"].attrs = {"unit": "kt", "description": "Speed Through Water"}  # (R7)

ds_HVPP["LEEWAY"] = (coords, np.zeros(shape))  # filled with zeros for the example
ds_HVPP["LEEWAY"].attrs = {"unit": "deg", "description": "LEEWAY angle"}  # (R7)

ds_HVPP.to_netcdf(filename, group="HVPP-PB", mode='a')
ds_HVPP.to_netcdf(filename, group="VPP-PB", mode='a')

# Note that "_FillValue" attribute is added on each Variables by xarray but is not part of POEM specifications.
