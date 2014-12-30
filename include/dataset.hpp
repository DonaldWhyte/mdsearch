/******************************************************************************

mdsearch - Lightweight C++ library implementing a collection of
           multi-dimensional search structures

File:        dataset.hpp
Description: Defines functionality for bulk-loading collections of points
             from arrays or text files.

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

#ifndef MDSEARCH_DATASET_H
#define MDSEARCH_DATASET_H

#include "types.hpp"
#include "point.hpp"
#include "boundary.hpp"
#include <boost/lexical_cast.hpp>
#include <fstream>

namespace mdsearch
{

    /** Sstores a collection of points with the same dimensionaliy.
     * Provides functionality to load points from std::vector objects or
     * text files. */
    template<int D, typename ELEM_TYPE>
    class Dataset
    {

    public:
        typedef std::vector< Point<D, ELEM_TYPE> > PointList;

        /** Add all given points to dataset. */
        void load(const PointList& newPoints);

        /*& Add all points in text file with given name to dataset.
         * Format of text file:
         * d n
         * p1_1 p1_2 ... p1_d
         * p2_1 p2_2 ... p2_d
         * ...
         * pn_1 pn_2 ... pn_d
         *
         * where 'd' is the dimensionality of the points and 'n'
         * is the number of points in the dataset. */
        void load(const std::string& filename);

        /** Compute minimum bounding hyper-rectangle that contains all
         * the points in the dataset. */
        Boundary<D, ELEM_TYPE> computeBoundary() const;

        /** Retrieve all points stored in dataset. */
        const PointList& getPoints() const;

    private:
        /** Contains all points in the dataset. */
        PointList m_points;

    };

    template<int D, typename ELEM_TYPE>
    void Dataset<D, ELEM_TYPE>::load(const PointList& newPoints)
    {
        // Pre-allocate memory in one sys call
        m_points.reserve(m_points.size() + newPoints.size());
        // Append given points to end of current point list
        m_points.insert(m_points.end(), newPoints.begin(), newPoints.end());
    }

    template<int D, typename ELEM_TYPE>
    void Dataset<D, ELEM_TYPE>::load(const std::string& filename)
    {
        // Open specified file and just do nothing if
        // the file does not exist
        std::ifstream file(filename.c_str());
        if (!file.is_open())
            return;

        // Read header information
        std::string numDimensionsStr;
        file >> numDimensionsStr;
        std::string numPointsStr;
        file >> numPointsStr;
        // Convert strings to integers
        int numDimensions = 0;
        int numPoints = 0;
        try
        {
            numDimensions = boost::lexical_cast<int>(numDimensionsStr);
            numPoints = boost::lexical_cast<int>(numPointsStr);
        }
        catch (boost::bad_lexical_cast& ex) // not integers -- invalid file!!
        {
            return;
        }
        // Only continue reading points if the points have at least
        // one dimension and there is at least one point in the dataset
        if (numDimensions < 1 || numPoints < 1)
            return;

        // Pre-allocate memory to store all the specified points
        m_points.reserve(m_points.size() + numPoints);
        // Treat the rest of lines as points
        ELEM_TYPE temp[D]; // temporary stores point's values
        for (unsigned int i = 0; (i < numPoints); i++)
        {
            for (unsigned int j = 0; (j < numDimensions); j++)
            {
                file >> temp[j];
            }
            m_points.push_back(Point<D, ELEM_TYPE>(temp));

            // If we have reached the end of the file, STOP and don't try
            // reading any more points
            if (file.eof())
            {
                break;
            }
        }
    }

    template<int D, typename ELEM_TYPE>
    Boundary<D, ELEM_TYPE> Dataset<D, ELEM_TYPE>::computeBoundary() const
    {
        Boundary<D, ELEM_TYPE> boundary(
            Interval<ELEM_TYPE>(0, 0)
        );

        if (!m_points.empty())
        {
            // Use first point for dimensionality and initial boundary
            const Point<D, ELEM_TYPE>& firstPoint = m_points[0];
            for (unsigned int d = 0; (d < D); d++)
            {
                boundary[d].min = firstPoint[d];
                boundary[d].max = firstPoint[d];
            }
            // Now search through rest of points in dataset to find
            // minimum and maximum values for each dimension
            for (typename PointList::const_iterator p = m_points.begin() + 1;
                (p != m_points.end()); p++)
            {
                for (unsigned int d = 0; (d < D); d++)
                {
                    if ((*p)[d] < boundary[d].min)
                    {
                        boundary[d].min = (*p)[d];
                    }
                    else if ((*p)[d] > boundary[d].max)
                    {
                        boundary[d].max = (*p)[d];
                    }
                }
            }
        }

        return boundary;
    }

    template<int D, typename ELEM_TYPE>
    inline
    const typename Dataset<D, ELEM_TYPE>::PointList&
        Dataset<D, ELEM_TYPE>::getPoints() const
    {
        return m_points;
    }

}

#endif