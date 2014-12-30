/******************************************************************************

mdsearch - Lightweight C++ library implementing a collection of
           multi-dimensional search structures

File:        point.hpp
Description: Contains class representing spatial points with an arbitrary
             number of dimensions.

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

    /** Represents spatial point with an arbitrary number of dimensions.
     * Can store any type of element, providing it supports the operations
     * required by Point's member functions. */
    template <int D, typename ELEM_TYPE>
    class Point
    {

    public:
        /** Constructs new point, but does not initialise its coordinates.
         * The initial values of the coordinates are undefined. */
        Point();

        /** Constructs new point, initialising each coordinate to the given
         * real value. */
        Point(ELEM_TYPE initialValue);

        /** Use given array of real values to initialise point's coordinates.
         * ASSUMPTION: 'initialValues' points to an array containing D real
         * numbers. If this is not the case, the behaviour is undefined. */
        Point(const ELEM_TYPE* initialValues);

        /** Return true if given point is equal to this one. */
        bool operator==(const Point& other) const;

        /** Return true if given point is not equal to this one. */
        bool operator!=(const Point& other) const;

        /** Retrieve value of dth coordinate. */
        ELEM_TYPE operator[](int d) const;

        /** Retrieve modifiable reference to dth coordinate. */
        ELEM_TYPE& operator[](int d);

        /** Retrieve point's coordinates as unmodifiable C-style array. */
        const ELEM_TYPE* asArray() const;

        /** Retrieve point's coordinates as modifiable C-style array. */
        ELEM_TYPE* asArray();

        /** Compute sum of all coordinates of point. */
        ELEM_TYPE sum() const;

        /** Output point's values to stream. */
        void print(std::ostream& out) const;

    private:
        /** Values of each coordinate. */
        ELEM_TYPE values[D];

    };

    template<int D, typename ELEM_TYPE>
    Point<D, ELEM_TYPE>::Point()
    {
    }

    template<int D, typename ELEM_TYPE>
    Point<D, ELEM_TYPE>::Point(ELEM_TYPE initialValue)
    {
        for (unsigned int d = 0; (d < D); d++)
        {
            values[d] = initialValue;
        }
    }

    template<int D, typename ELEM_TYPE>
    Point<D, ELEM_TYPE>::Point(const ELEM_TYPE* initialValues)
    {
        memcpy(values, initialValues, sizeof(ELEM_TYPE) * D);
    }

    template<int D, typename ELEM_TYPE>
    inline
    bool Point<D, ELEM_TYPE>::operator==(const Point& other) const
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

    template<int D, typename ELEM_TYPE>
    inline
    bool Point<D, ELEM_TYPE>::operator!=(const Point& other) const
    {
        return !(*this == other);
    }

    template<int D, typename ELEM_TYPE>
    inline
    ELEM_TYPE Point<D, ELEM_TYPE>::operator[](int d) const
    {
        return values[d];
    }

    template<int D, typename ELEM_TYPE>
    inline
    ELEM_TYPE& Point<D, ELEM_TYPE>::operator[](int d)
    {
        return values[d];
    }

    template<int D, typename ELEM_TYPE>
    inline
    const ELEM_TYPE* Point<D, ELEM_TYPE>::asArray() const
    {
        return &values[0];
    }

    template<int D, typename ELEM_TYPE>
    inline
    ELEM_TYPE* Point<D, ELEM_TYPE>::asArray()
    {
        return &values[0];
    }

    template<int D, typename ELEM_TYPE>
    inline
    ELEM_TYPE Point<D, ELEM_TYPE>::sum() const
    {
        ELEM_TYPE s = 0;
        for (unsigned int d = 0; (d < D); d++)
        {
            s += values[d];
        }
        return s;
    }

    template<int D, typename ELEM_TYPE>
    inline
    void Point<D, ELEM_TYPE>::print(std::ostream& out) const
    {
        out << "(";
        for (int i = 0; (i < D - 1); i++)
        {
            out << values[i] << ",";
        }
        out << values[D - 1] << ")";
    }

    template<int D, typename ELEM_TYPE>
    std::ostream& operator<<(std::ostream& out,
                             const Point<D, ELEM_TYPE>& point)
    {
        point.print(out);
        return out;
    }

}

#endif