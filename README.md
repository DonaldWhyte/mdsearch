#mdsearch

Lightweight, header-only C++ library which implements a collection of multi-dimensional search structures.

###Dependencies

**mdsearch** requires the following header-only Boost libraries:
* Boost.Unordered
* Boost.Functional/Hash
* Boost.Lexical_Cast

###API

Each index structure is represented as a template class, where the templated parameter is the dimensionality of the data the structure is storing. Each index structure class implements the following methods:
* ```bool insert(point)``` -- inserts unique point into the structure. Returns true if point insertion was successful and false if point is already being stored and has not been inserted.
* ```bool remove(point)``` -- deletes point from structure. Returns true if the point was found and deleted successful and false if the point was not found.
* ```bool query(point)``` -- returns true if point is being stored in structure and false otherwise.

###Examples

The library comes with a program that generates random points and performs a set of correctness and performance tests on each index structure. This program is contained within the ```test_structures.cpp``` file. This program is an example on how to construct and use the implemented index structures.

```test_core.cpp``` contains a program that tests the correctness of the library's core data types. This show examples of how to construct points and boundaries compatible with the index structures.