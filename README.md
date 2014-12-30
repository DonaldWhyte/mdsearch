#mdsearch

Lightweight, header-only C++ library which implements a collection of
multi-dimensional search structures.

###Dependencies

**mdsearch** requires the following header-only Boost libraries:
* Boost.Unordered
* Boost.Functional/Hash
* Boost.Lexical_Cast

The minimum supported version of Boost is 1.41.

###API

Each index structure is represented as a templated class, where the template
parameter is the dimensionality of the data the structure is storing. These
classes implement the following methods:
* ```bool insert(point)``` -- insert unique point into the structure. Returns
true if point insertion was successful and false if point is already being
stored and has not been inserted.
* ```bool remove(point)``` -- delete point from structure. Returns true if the
point was found and deleted successful and false if the point was not found.
* ```bool query(point)``` -- return true if point is being stored in structure
and false otherwise.

###Examples

The library comes with a program that generates random points and performs a
set of correctness and performance tests on each index structure. This program
is contained within the ```test_structures.cpp``` file, and shows examples of
how to construct and use the implemented index structures.

```test_core.cpp``` contains a program that tests the correctness of the
library's core data types. This shows how to construct points and boundaries
compatible with the index structures.

###Storage Type Requirements

Generally, floating-point types are stored in multi-dimensional points.
However, the Point class and all the structures in mdsearch can store any
type. The type to store as coordinates in the Point class is specified using
the second template parameter, `ELEM_TYPE`, of the library's classes.

 for an arbitrary type
To allow an arbitrary type `ELEM_TYPE` to be used with the structures, the
following free functions must be implemented:
* `bool operator==(const ELEM_TYPE&, const ELEM_TYPE&)`
* `bool operator!=(const ELEM_TYPE&, const ELEM_TYPE&)`
* `bool operator<=(const ELEM_TYPE&, const ELEM_TYPE&)`
* `bool operator>=(const ELEM_TYPE&, const ELEM_TYPE&)`
* `bool operator<(const ELEM_TYPE&, const ELEM_TYPE&)`
* `bool operator>(const ELEM_TYPE&, const ELEM_TYPE&)`
* `ELEM_TYPE operator+(const ELEM_TYPE&, const ELEM_TYPE&)`
* `ELEM_TYPE operator-(const ELEM_TYPE&, const ELEM_TYPE&)`
* `ELEM_TYPE operator*(const ELEM_TYPE&, const ELEM_TYPE&)`
* `ELEM_TYPE operator/(const ELEM_TYPE&, const ELEM_TYPE&)`
* `ELEM_TYPE std::min(const ELEM_TYPE&, const ELEM_TYPE&)`
* `ELEM_TYPE std::max(const ELEM_TYPE&, const ELEM_TYPE&)`
The following member functions must also be implemented:
* `ELEM_TYPE& ELEM_TYPE::operator=(const ELEM_TYPE&)`
* `ELEM_TYPE& ELEM_TYPE::operator+=(const ELEM_TYPE&)`
* `ELEM_TYPE& ELEM_TYPE::operator-=(const ELEM_TYPE&)`
* `ELEM_TYPE& ELEM_TYPE::operator*=(const ELEM_TYPE&)`
* `ELEM_TYPE& ELEM_TYPE::operator/=(const ELEM_TYPE&)`
