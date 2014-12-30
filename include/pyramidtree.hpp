/******************************************************************************

mdsearch - Lightweight C++ library implementing a collection of
           multi-dimensional search structures

File:        pyramidtree.hpp
Description: Implements the Pyramid Tree from Berchtold et al.'s 1998 paper.
             Instead of using a B+-tree as the underlying one-dimensional index
             structure, a hash map is used instead.

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

#ifndef MDSEARCH_PYRAMIDTREE_H
#define MDSEARCH_PYRAMIDTREE_H

#include "hashstruct.hpp"
#include "boundary.hpp"

// Only define if you want a hack which causes the Pyramid Tree hasher
// to ignore dimensions when a point is at the min or max boundaries
// for that dimension
#define BOUNDARY_VALUE_HACK 1

namespace mdsearch
{

    template<int D, typename ELEM_TYPE>
    class PyramidTree : public HashStructure<D, ELEM_TYPE>
    {

    public:
        PyramidTree(const Boundary<D, ELEM_TYPE>& boundary);

        void clear(const Boundary<D, ELEM_TYPE>& newBoundary);

    protected:
        static const Real MAX_BUCKET_NUMBER = 30000000000;

        /* Normalise value into 0-1 range based on min-max interval. */
        Real normaliseCoord(Real coord, Real min, Real max);
        /* Compute Pyramid height of a point, for a specific pair of
         * pyramid (that are both for the same dimension). */
        Real pyramidHeight(Real coord, Real min, Real max);
        /* Compute pyramid value of the given point, using the
         * Pyramid-technique. */
        virtual HashType hashPoint(const Point<D, ELEM_TYPE>& p);

        // Entire region of space the Pyramid tree covers
        // (points outside this region are ignored)
        Boundary<D, ELEM_TYPE> boundary;
        // Interval between buckets
        Real bucketInterval;

    };

    template<int D, typename ELEM_TYPE>
    PyramidTree<D, ELEM_TYPE>::PyramidTree(
        const Boundary<D, ELEM_TYPE>& boundary)
    : boundary(boundary)
    {
        // Compute the interval between buckets 
        bucketInterval = static_cast<Real>( MAX_BUCKET_NUMBER / (D * 2) );
        bucketInterval = floor(bucketInterval);
    }

    template<int D, typename ELEM_TYPE>
    void PyramidTree<D, ELEM_TYPE>::clear(
        const Boundary<D, ELEM_TYPE>& newBoundary)
    {
        HashStructure<D, ELEM_TYPE>::clear();
        boundary = newBoundary;
    }

    template<int D, typename ELEM_TYPE>
    inline
    Real PyramidTree<D, ELEM_TYPE>::normaliseCoord(Real coord,
                                                   Real min, Real max)
    {
        return (coord - min) / (max - min);
    }
    
    template<int D, typename ELEM_TYPE>
    inline
    Real PyramidTree<D, ELEM_TYPE>::pyramidHeight(Real coord,
                                                  Real min, Real max)
    {
        return std::abs(0.5f - normaliseCoord(coord, min, max));
    }

    template<int D, typename ELEM_TYPE>
    HashType PyramidTree<D, ELEM_TYPE>::hashPoint(const Point<D, ELEM_TYPE>& p)
    {
        int index = 0;
        int dMax = 0;
        Real dMaxHeight = pyramidHeight(p[0],
            boundary[0].min, boundary[0].max);
        for (int d = 1; (d < D); d++)
        {
            Real currentHeight = pyramidHeight(p[d],
                boundary[d].min, boundary[d].max);
            #ifdef BOUNDARY_VALUE_HACK
            if (compare(currentHeight, 0.5f) == 0)
            {
                continue;
            }
            #endif

            if (dMaxHeight < currentHeight)
            {
                dMax = d;
                dMaxHeight = currentHeight;
            }
        }

        Real normalisedCoord = normaliseCoord(p[dMax],
            boundary[dMax].min, boundary[dMax].max);
        if (normalisedCoord < 0.5f)
        {
            index = dMax; // pyramid lower than central point
        }
        else 
        {
            index = dMax + D; // pyramid higher than central point
        }

        return (index + dMaxHeight) * bucketInterval;            
    }

}

#endif