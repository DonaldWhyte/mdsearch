/******************************************************************************

mdsearch - Lightweight C++ library implementing a collection of
           multi-dimensional search structures

File:        types.hpp
Description: Defines the core data types used throughout the library.

*******************************************************************************

The MIT License (MIT)

Copyright (c) 2014 Donald Whyte

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

******************************************************************************/

#ifndef MDSEARCH_TYPES_H
#define MDSEARCH_TYPES_H

#include <vector>
#include <cmath>

namespace mdsearch
{

    /**
     * Real numbers are the smallest component of multi-dimensional search.
     * This typedef defines which data type is used to represent these
     * components. */
    typedef float Real;

    /**
     * Defined error tolerance for floating point comparisons.
     */
    static const Real EPSILON = 1.0e-7;

    /** Compare two reals subject to an error tolerance.
     * Return -1, 0, 1 if 't' is less than, approximately equal to, or greater
     * than, 'base' respectively. */
    inline int compare(Real t, Real base)
    {
        if (std::fabs(t - base) < EPSILON)
        {
            return 0;
        }
        if (t < base)
        {
            return -1;
        }
        else
        {
            return 1;
        }
    }

    /** Data type used for one-dimensional hash values of points.
     * All hash-based index structures should use this, unless there's
     * a very good reason not to. */
    typedef long HashType;

}

#endif