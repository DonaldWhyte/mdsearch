/******************************************************************************

mdsearch - Lightweight C++ library implementing a collection of
           multi-dimensional search structures

File:        bucket_kdtree_strategies.hpp
Description: Contains various methods for selecting cutting dimensions and
			 cutting values (also called 'pivots') for specific point
			 distributions. These strategies can be used by the BucketKDTree
			 structure.

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

#ifndef MDSEARCH_BUCKET_KDTREE_STRATEGIES_H
#define MDSEARCH_BUCKET_KDTREE_STRATEGIES_H

#include "point.hpp"
#include <vector>

namespace mdsearch
{

	/** Contains various methods for selecting a cutting dimension for
	 * point distributions. */
	template<int D, typename ELEM_TYPE>
	class CuttingDimensionStrategies
	{

	public:
		/** Return dimension with largest range of values. */
		static int dimensionWithHighestRange(
			const std::vector< Point<D, ELEM_TYPE> >& points);

	private:
		/** Compute (max - min) range of values for dimension d. */
		static ELEM_TYPE rangeOfDimension(int d,
			const std::vector< Point<D, ELEM_TYPE> >& points);

	};

	/** Contains various methods for selecting a cutting value for
	 * point distributions. */
	template<int D, typename ELEM_TYPE>
	class CuttingValueStrategies
	{

	public:
		/** Return average value of dth coordinate of given points */
		static ELEM_TYPE averageOfDimension(int d,
			const std::vector< Point<D, ELEM_TYPE> >& points);

	};

    template<int D, typename ELEM_TYPE>
    inline
    ELEM_TYPE CuttingDimensionStrategies<D, ELEM_TYPE>::rangeOfDimension(
    	int d, const std::vector< Point<D, ELEM_TYPE> >& points)
    {
        if (points.empty())
        {
            return 0;
        }
        else
        {
            ELEM_TYPE min = points[0][d];
            ELEM_TYPE max = min;
            for (typename std::vector< Point<D, ELEM_TYPE> >::const_iterator
                iter = points.begin(); iter != points.end(); ++iter)
            {
                ELEM_TYPE val = (*iter)[d];
                if (val < min)
                {
                    min = val;
                }
                else if (val > max)
                {
                    max = val;
                }
            }

            return (max - min);
        }
    }

    template<int D, typename ELEM_TYPE>
    inline
    int CuttingDimensionStrategies<D, ELEM_TYPE>::dimensionWithHighestRange(
        const std::vector< Point<D, ELEM_TYPE> >& points)
    {
        int chosenDim = 0;
        ELEM_TYPE maxRange = rangeOfDimension(0, points);

        for (int d = 1; d < D; ++d)
        {
            ELEM_TYPE range = rangeOfDimension(d, points);
            if (range > maxRange)
            {
                chosenDim = d;
                maxRange = range;
            }
        }

        return chosenDim;
    }

    template<int D, typename ELEM_TYPE>
    inline
    ELEM_TYPE CuttingValueStrategies<D, ELEM_TYPE>::averageOfDimension(int d,
        const std::vector< Point<D, ELEM_TYPE> >& points)
    {
        ELEM_TYPE sum = 0;
        for (typename std::vector< Point<D, ELEM_TYPE> >::const_iterator iter
            = points.begin(); iter != points.end(); ++iter)
        {
            sum += (*iter)[d];
        }
        return sum / points.size();
    }

}

#endif