/******************************************************************************

mdsearch - Lightweight C++ library implementing a collection of
           multi-dimensional search structures

File:        types.hpp
Description: Deifnes the core data types used throughout the library.

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

#include <iostream>
#include <vector>
#include <cmath>
#include <cstring>

namespace mdsearch
{

	// Real numbers are the smallest component of multi-dimensional search.
	// This typedef defines which data type is used to represent these components
	typedef float Real;
	typedef std::vector<Real> RealList;

	/* Defined error tolerance for floating point comparisons. */
	static const Real EPSILON = 1.0e-7;
	/* Compare two reals subject to an error tolerance.
	 * Return -1, 0, 1 respectively if 't' is less than,
	 * approximately equal to, or greater than, 'base'. */
	inline int compare(Real t, Real base)
	{
		if (std::fabs(t - base) < EPSILON)
			return 0;
		if (t < base)
			return -1;
		return 1;
	}

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
				if (compare(values[d], other.values[d]) != 0) 
					return false;
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
				s += values[d];
			return s;	
		}

		Real values[D];

	};

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

	/* std::ostream stream operator overloads for easily printing points and boundaries. */
	template<int D>
	std::ostream& operator<<(std::ostream& out, const Point<D>& point)
	{
		out << "(";
		for (int i = 0; (i < D - 1); i++)
			out << point[i] << ",";
		out << point[D - 1] << ")";
		return out;
	}

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
			out << boundary[d] << ",";
		out << boundary[D - 1];
		out << ")";
		return out;;
	}	

}

#endif