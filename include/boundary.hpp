/******************************************************************************

mdsearch - Lightweight C++ library implementing a collection of
           multi-dimensional search structures

File:        boundary.hpp
Description: Contains class representing spatial boundaries with an arbitrary
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

#ifndef MDSEARCH_BOUNDARY_H
#define MDSEARCH_BOUNDARY_H

#include <iostream>

namespace mdsearch
{

    /* Represents interval of values. Can be used with any type. */
    template<typename ELEM_TYPE>
    struct Interval
    {

        /** Instantiates an interval, but does not initialise the
         * minimum and maximum values. */
        Interval();

        /** Instantiates an interval, initialising its min and max
         * bounds using the given values. */
        Interval(ELEM_TYPE min, ELEM_TYPE max);

        /** Output interval's min and max value to stream. */
        void print(std::ostream& out) const;

        /** Lower bound of interval. */
        ELEM_TYPE min;
        /** Upper bound of interval. */
        ELEM_TYPE max;

    };

    /** Represents spatial boundary with an arbitrary number of dimensions.
     * Can use any type of element for the min/max boundary values, providing
     * the type supports the operations required by Boundary's member
     * functions. */
    template<int D, typename ELEM_TYPE>
    class Boundary
    {

    public:
        /** Constructs new boundary, but does not initialise its intervals.
         * The initial values of the intervals are undefined. */
        Boundary();

        /** Constructs new boundary, initialising each interval to the given
         * interval. */
        Boundary(const Interval<ELEM_TYPE>& initialInterval);

        /** Use given array of intervals to initialise boundary's intervals.
         * ASSUMPTION: 'initialIntervals' points to an array containing D
         * Interval objects. If this is not the case, the behaviour is
         * undefined. */
        Boundary(const Interval<ELEM_TYPE>* initialIntervals);

        /** Retrieve interval (min and max value) of dth dimension. */
        const Interval<ELEM_TYPE>& operator[](int d) const;

        /** Retrieve modifiable reference to interval (min and max value) of
         * dth dimension. */
        Interval<ELEM_TYPE>& operator[](int d);

        /** Output all the boundary's intervals to stream. */
        void print(std::ostream& out) const;

    private:
        /** Minimum and maximum values for each dimension. */
        Interval<ELEM_TYPE> m_intervals[D];

    };

    template<typename ELEM_TYPE>
    Interval<ELEM_TYPE>::Interval()
    {
    }

    template<typename ELEM_TYPE>
    Interval<ELEM_TYPE>::Interval(ELEM_TYPE min, ELEM_TYPE max)
    : min(min), max(max)
    {
    }

    template<typename ELEM_TYPE>
    inline
    void Interval<ELEM_TYPE>::print(std::ostream& out) const
    {
        out << "[" << min << ":" << max << "]";
    }

    template<int D, typename ELEM_TYPE>
    Boundary<D, ELEM_TYPE>::Boundary()
    {
    }

    template<int D, typename ELEM_TYPE>
    Boundary<D, ELEM_TYPE>::Boundary(
        const Interval<ELEM_TYPE>& initialInterval)
    {
        for (unsigned int d = 0; (d < D); d++)
        {
            m_intervals[d] = initialInterval;
        }
    }

    template<int D, typename ELEM_TYPE>
    Boundary<D, ELEM_TYPE>::Boundary(
        const Interval<ELEM_TYPE>* initialIntervals)
    {
        memcpy(m_intervals, initialIntervals, sizeof(Interval<ELEM_TYPE>) * D);
    }

    template<int D, typename ELEM_TYPE>
    inline
    const Interval<ELEM_TYPE>& Boundary<D, ELEM_TYPE>::operator[](int d) const
    {
        return m_intervals[d];
    }

    template<int D, typename ELEM_TYPE>
    inline
    Interval<ELEM_TYPE>& Boundary<D, ELEM_TYPE>::operator[](int d)
    {
        return m_intervals[d];
    }

    template<int D, typename ELEM_TYPE>
    inline
    void Boundary<D, ELEM_TYPE>::print(std::ostream& out) const
    {
        out << "(";
        for (unsigned int d = 0; (d < D - 1); d++)
        {
            out << m_intervals[d] << ",";
        }
        out << m_intervals[D - 1];
        out << ")";
    }

    template<typename ELEM_TYPE>
    std::ostream& operator<<(std::ostream& out,
                             const Interval<ELEM_TYPE>& interval)
    {
        interval.print(out);
        return out;

    }

    template<int D, typename ELEM_TYPE>
    std::ostream& operator<<(std::ostream& out,
                             const Boundary<D, ELEM_TYPE>& boundary)
    {
        boundary.print(out);
        return out;
    }

}

#endif