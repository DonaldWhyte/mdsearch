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
#include <boost/lexical_cast.hpp>
#include <fstream>

namespace mdsearch
{

    /* Class which stores a collection of points with the same dimensionaliy.
     * Provides functionality to load points from std::vector objects or
     * text files. */
    template<int D>
    class Dataset
    {

    public:
        typedef std::vector< Point<D> > PointList;

        /* Add all given points to dataset. */
        void load(const PointList& newPoints)
        {
            // Pre-allocate memory in one sys call
            points.reserve(points.size() + newPoints.size());
            // Append given points to end of current point list
            points.insert(points.end(), newPoints.begin(), newPoints.end());
        }

        /* Add all points in text file with given name to dataset. 
         * Format of text file:
         * d n
         * p1_1 p1_2 ... p1_d
         * p2_1 p2_2 ... p2_d
         * ...
         * pn_1 pn_2 ... pn_d
         *
         * where 'd' is the dimensionality of the points and 'n'
         * is the number of points in the dataset. */
        void load(const std::string& filename)
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
            points.reserve(points.size() + numPoints);
            // Treat the rest of lines as points
            Real temp[D]; // temporary stores point's values
            for (unsigned int i = 0; (i < numPoints); i++)
            {
                for (unsigned int j = 0; (j < numDimensions); j++)
                {
                    file >> temp[j];
                }
                points.push_back(Point<D>(temp));

                // If we have reached the end of the file, STOP and don't try
                // reading any more points
                if (file.eof())
                    break;
            }
        }

        /* Compute minimum bounding hyper-rectangle that contains all 
         * the points in the dataset. */
        Boundary<D> computeBoundary() const
        {
            Boundary<D> boundary(Interval(0, 0));

            if (!points.empty())
            {
                // Use first point for dimensionality and initial boundary
                const Point<D>& firstPoint = points[0];
                for (unsigned int d = 0; (d < D); d++)
                {
                    boundary[d].min = firstPoint[d];
                    boundary[d].max = firstPoint[d];
                }
                // Now search through rest of points in dataset to find
                // minimum and maximum values for each dimension
                for (typename PointList::const_iterator p = points.begin() + 1; (p != points.end()); p++)
                {
                    for (unsigned int d = 0; (d < D); d++)
                    {
                        if ((*p)[d] < boundary[d].min)
                            boundary[d].min = (*p)[d];
                        else if ((*p)[d] > boundary[d].max)
                            boundary[d].max = (*p)[d];
                    }
                }
            }

            return boundary;
        }

        /* Retrieve all points stored in dataset. */
        const PointList& getPoints() const
        {
            return points;
        }

    private:
        PointList points;

    };

}

#endif