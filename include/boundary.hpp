/******************************************************************************

mdsearch - Lightweight C++ library implementing a collection of
           multi-dimensional search structures

File:        boundary.hpp
Description: TODO

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

#ifndef MDSEARCH_BOUNDARY_H
#define MDSEARCH_BOUNDARY_H

#include <iostream>

namespace mdsearch
{

    /* One-dimensional interval. */
    struct Interval
    {

        /* Instantiates an interval, but does not initialise the
         * minimum and maximum values. */
        Interval()
        {
        }

        /* Instantiates an interval, initialising its min and max
         * bounds using the given values. */
        Interval(Real min, Real max) : min(min), max(max)
        {
        }

        // Lower and upper bounds of the interval
        Real min;
        Real max;

    };

    /* Boundary in D_dimensional space. This structure contains D
     * one-dimensional intervals. */
    template<int D>
    struct Boundary
    {

        /* Constructs new boundary, but does not initialise its intervals.
         * The initial values of the intervals are therefore undefined. */
        Boundary()
        {
        }

        /* Constructs new boundary, initialising each interval to the given interval. */
        Boundary(const Interval& initialInterval)
        {
            for (unsigned int d = 0; (d < D); d++)
            {
                intervals[d] = initialInterval;
            }
        }

        /* Use given array of intervals to initialise boundary's intervals.
         * ASSUMPTION: 'initialIntervals' points to an array containing D
         * Interval objects. */        
        Boundary(const Interval* initialIntervals)
        {
            memcpy(intervals, initialIntervals, sizeof(Interval) * D);
        }

        // Accessor/mutator for individual intervals
        inline const Interval& operator[](int d) const { return intervals[d]; }
        inline Interval& operator[](int d) { return intervals[d]; }

        // Minimum and maximum values for each dimension
        Interval intervals[D];

    };


    std::ostream& operator<<(std::ostream& out, const Interval& interval)
    {
        out << "[" << interval.min << ":" << interval.max << "]";
        return out;
    }

    template<int D>
    std::ostream& operator<<(std::ostream& out, const Boundary<D>& boundary)
    {
        out << "(";
        for (unsigned int d = 0; (d < D - 1); d++)
        {
            out << boundary[d] << ",";
        }
        out << boundary[D - 1];
        out << ")";
        return out;;
    }

}

#endif