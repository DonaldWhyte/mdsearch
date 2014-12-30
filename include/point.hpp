/******************************************************************************

mdsearch - Lightweight C++ library implementing a collection of
           multi-dimensional search structures

File:        point.hpp
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

#ifndef MDSEARCH_POINT_H
#define MDSEARCH_POINT_H

#include <cstring>
#include <iostream>

namespace mdsearch
{

    /* Structure representing points in D-dimensional space. */
    template <int D>
    struct Point
    {

        /* Constructs new point, but does not initialise its coordinates.
         * The initial values of the coordinates are therefore undefined. */
        Point()
        {
        }

        /* Constructs new point, initialising each coordinate
         * to the given real value. */
        Point(Real initialValue)
        {
            for (unsigned int d = 0; (d < D); d++)
            {
                values[d] = initialValue;
            }
        }

        /* Use given array of real values to initialise point's coordinates.
         * ASSUMPTION: 'initialValues' points to an array containing D
         * real numbers. */
        Point(const Real* initialValues)
        {
            memcpy(values, initialValues, sizeof(Real) * D);
        }

        /* Point equality and inequality. */
        inline bool operator==(const Point& other) const
        {
            for (unsigned int d = 0; (d < D); d++)
            {
                if (compare(values[d], other.values[d]) != 0) 
                {
                    return false;
                }
            }
            return true;
        }
        inline bool operator!=(const Point& other) const
        {
            return !(*this == other);
        }

        /* Individual coordinate accessors. */
        inline Real operator[](int index) const
        {
            return values[index];
        }
        inline Real& operator[](int index)
        {
            return values[index];
        }

        /* Compute sum of all coordinates of point. */
        inline Real sum() const
        {
            Real s = 0;
            for (unsigned int d = 0; (d < D); d++)
            {
                s += values[d];
            }
            return s;    
        }

        Real values[D];

    };

    /* std::ostream stream operator overloads for easy display of points and boundaries. */
    template<int D>
    std::ostream& operator<<(std::ostream& out, const Point<D>& point)
    {
        out << "(";
        for (int i = 0; (i < D - 1); i++)
        {
            out << point[i] << ",";
        }
        out << point[D - 1] << ")";
        return out;
    }

}

#endif