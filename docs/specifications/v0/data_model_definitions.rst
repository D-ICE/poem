.. _data_model_definitions_v0:

Data Model Definitions (v0)
===========================

**POEM File Format Specifications** RULES defined in the next section rely on different notions inherited from *NetCDF-4
Data Model*. In the following, we provide basic definitions for these notions.

**Dataset**
    A data structure that contains *Dimensions*, *Variables* and *Attributes*. It can be roughly seen as the content of a
    *NetCDF-4* file for the current specification.

**Dimensions**
    A *Dimension* is used to represent a physical dimension. A *Dimension* has both a name and a length.

**Variables**
    A *Variable* represents an array of values of the same type. It has a name, a datatype and a shape described by
    its list of *Dimensions*.

**Coordinate Variables**
    A *Coordinate Variable* IS A *Variable* that is associated to a *Dimension*, with the same name. It provides a number of
    numerical coordinate values equal to the length of the corresponding *Dimension*.

**Attributes**
    *Attributes* are used to store data about the data (or metadata). Attributes can be specified on any *NetCDF-4 object*,
    i.e. on *Dataset*, *Dimension*s or *Variables*.


