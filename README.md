# POEM

**Performance pOlar Exchange forMat**

![PyPI - Python Version](https://img.shields.io/pypi/pyversions/pypoem)
[![PyPI version](https://badge.fury.io/py/pypoem.svg)](https://badge.fury.io/py/pypoem)

C++ library and Python bindings defining an exchange format for performance polar in marine engineering, 
wind assisted vessels performance prediction, weather routing optimisation, embedded control systems, 
training simulators.

<!-- TOC -->

- [Links](#links)
- [Introduction](#introduction)
- [POEM in a nuttshell](#poem-in-a-nuttshel)
- [Python interface](#python-interface)
  - [Requirements](#requirements)
  - [Install from Pypi](#install-from-pypi)
  - [Install from sources](#install-from-sources)
  - [Basic usage](#basic-usage)
- [Integrating the C++ library](#integrating-the-c-library)
  - [Requirements](#requirements-1)
  - [CMake](#cmake)
  - [Basic usage](#basic-usage-1)
- [Current limitations](#current-limitations)

[//]: # (    - [How to cite poem]&#40;#how-to-cite-poem&#41;)

[//]: # (    - [Contributing]&#40;#contributing&#41;)
    

<!-- TOC -->


## Links

* **Github repository:** https://github.com/D-ICE/poem
* **API Documentation:** 
* **POEM Documentation:** https://dice-poem.readthedocs.io/en/latest/
* **D-ICE ENGINEERING:** https://www.dice-engineering.com/en

## Introduction

* what is a VPP
* what is a digital twin
* what is a performance polar
* the different type of polars
* how a polar is used

## POEM in a nuttshel

* why poem
* what is poem
* is poem for me


## Python interface

The pypoem package that wraps poem is pypoem. It does not currently give acces to the whole C++ API but feature are 
progressively ported to Python.

### Requirements

pypoem is tested with Python version >= 3.9.

### Install from Pypi

pypoem is available on Pypi, so it can be installed as:

```console
$ pip install pypoem
```

### Install from sources

pypoem relies on [scikit-build-core](https://scikit-build-core.readthedocs.io/en/latest/index.html) for the packaging.
Source installation is realised by running the following command int the repository root directory:

```console
$ pip install .
```

### Basic usage

Here is a simple example of the creation of a MPPP Polar with one PolarTable and its writing to a netCDF file
compliant with POEM file format thanks the of POEM library.

```python
from pypoem import pypoem
import numpy as np

# Declaring three dimensions
STW = pypoem.make_dimension("STW", "kt", "Speed Through Water")
TWS = pypoem.make_dimension("TWS", "kt", "True Wind Speed")
TWA = pypoem.make_dimension("TWA", "deg", "True Wind Angle")

# Creating a DimensionSet
dimension_set = pypoem.make_dimension_set((STW, TWS, TWA))

# Creating a DImensionGrid and filling the dimensions with values
dimension_grid = pypoem.make_dimension_grid(dimension_set)
dimension_grid.set_values("STW", np.linspace(8, 20, 13))
dimension_grid.set_values("TWS", np.linspace(0, 40, 9))
dimension_grid.set_values("TWA", np.linspace(0, 180, 13))

# Creating a Polar of type MPPP (Motor only Power Prediction)
polar_MPPP = pypoem.make_polar("MPPP", pypoem.MPPP, dimension_grid)

# Creating a PolarTable from the MPPP Polar
BrakePower = polar_MPPP.create_polar_table_double("BrakePower", "kW", "Brake Power", 
                                                  pypoem.POEM_DOUBLE)

# Generating a dummy NDArray with ones and the same shape as the DimensionGrid
# Note that NDArrays must be arranged in a row major order with respect the DimensionSet
# of the DimensionGrid. If you think in terms of nested for loop with Dimensions, 
# the last Dimension (here TWA) is moving fastest (most inner loop).
brake_power_data = np.ones(dimension_grid.shape())

# Setting the BrakePower PolarTable with the array
BrakePower.set_values(brake_power_data)
assert(np.all(brake_power_data == BrakePower.array()))

# Writing to netCDF
pypoem.to_netcdf(polar_MPPP, "polar_basic.nc")

```

* Reading a POEM file
* More complex example
* 


Work In Progress

## Integrating the C++ library

Work In Progress

### Requirements

Work In Progress

### CMake
Work In Progress

### Basic usage

Work In Progress

## Current limitations

POEM has been primarily developed under Linux OS. Port to other platforms is expected to come in the future but is not
a current top priority. Contributors for a Windows port are welcome :)

[//]: # (## How to cite poem)

[//]: # (## Contributing)




Latest standard is available at:
https://dice-poem.readthedocs.io/en/latest/
