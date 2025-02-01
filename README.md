# POEM

**Performance pOlar Exchange forMat**

C++ library and exchange format for performance polar in marine engineering, wind assisted vessels performance prediction,
weather routing optimisation, embedded control systems, training simulators.

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
* **POEM Specifications:** https://dice-poem.readthedocs.io/en/latest/
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
