/******************************************************************************

mdsearch - Lightweight C++ library implementing a collection of
           multi-dimensional search structures

File:        multigrid.hpp
Description: Contains an implementation of the Multigrid Tree.

             The Multigrid Tree decomposes the data space into a uniform grid
             by cutting each dimension into B intervals. A cell is defined by
             the interval of each dimension it is contained in, meaning there
             are a total of B^d cells, where d is the number of dimensions.

             A point is stored in a bucket that corresponds to the cell
             it is contained in. In the best case, insertions, deletions
             and point queries can be performed in O(d) time. In the worst
             case, all points will be in the same cell, meaning these
             operations will take O(n) time.

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

#include "types.hpp"
#include "point.hpp"
#include "boundary.hpp"
#include <stack>
#include <boost/unordered_map.hpp>
#include <algorithm>

namespace mdsearch
{

    struct MultigridNode
    {
        bool isLeaf;
        // Leaf information
        std::vector<int> pointIndices;
        // Non-leaf information
        boost::unordered_map<HashType, MultigridNode>* children;

        MultigridNode();
        ~MultigridNode();

        void addPoint(int pointIndex);
        bool removePoint(int pointIndex);
        std::size_t numPoints() const;

    };

    template<int D>
    class Multigrid
    {

    public:
        Multigrid(const Boundary<D>& boundary,
            Real intervalsPerDimension = 1000000000,
            int bucketSize = 8);

        void clear(const Boundary<D>& newBoundary);
        bool insert(const Point<D>& p);
        bool query(const Point<D>& p);
        bool remove(const Point<D>& p);

        int numPoints() const;
        int numBuckets() const;
        double averageBucketSize() const;

    private:
        typedef boost::unordered_map<HashType, MultigridNode> BucketMap;

        /* Return total number of buckets in given map,by recursively 
         * searching through it. */
        int numBuckets(const BucketMap& map) const;
        /* Insert point into given bucket. The given dimension is used to hash
         * the point. */
        bool insertIntoBucket(const Point<D>& p,
                              int currentDim,
                              MultigridNode* currentBucket);
        /* Normalise value into 0-1 range based on min-max interval. */
        Real normaliseCoord(Real coord, Real min, Real max);
        /* Compute pyramid value of the given point, using the original
         * Pyramid-technique. */
        HashType hashPoint(const Point<D>& p, int d);
        /* Retrieve pointer to bucket that contains points that have the 
         * given hash value. */
        MultigridNode* getBucketPointer(BucketMap* map, Real hashValue);

        Boundary<D> boundary;
        Real intervalsPerDimension;
        int bucketSize;

        BucketMap rootBuckets;
        std::vector< Point<D> > points;
        std::stack<int> unusedIndices;

    };

    MultigridNode::MultigridNode() : isLeaf(true), children(NULL)
    {
    }

    MultigridNode::~MultigridNode()
    {
        delete children;
    }

    inline
    void MultigridNode::addPoint(int pointIndex)
    {
        pointIndices.push_back(pointIndex);
    }

    inline
    bool MultigridNode::removePoint(int pointIndex)
    {
        pointIndices.erase(
            std::remove(pointIndices.begin(), pointIndices.end(), pointIndex),
            pointIndices.end()
        );
        return true;
    }

    inline
    std::size_t MultigridNode::numPoints() const
    {
        return pointIndices.size();
    }

    template<int D>
    Multigrid<D>::Multigrid(const Boundary<D>& boundary,
        Real intervalsPerDimension, int bucketSize) :
        boundary(boundary),
        intervalsPerDimension(intervalsPerDimension),
        bucketSize(bucketSize)
    {
    }

    template<int D>
    inline
    void Multigrid<D>::clear(const Boundary<D>& newBoundary)
    {
        boundary = newBoundary;
        rootBuckets = BucketMap();
    }

    template<int D>
    bool Multigrid<D>::insert(const Point<D>& p)
    {
        int currentDim = 0;
        HashType pyVal = hashPoint(p, currentDim);

        // Find the next bucket to traverse
        MultigridNode* nextBucket = getBucketPointer(&rootBuckets, pyVal);
        // If bucket not found, create new bucket and insert point into it
        if (!nextBucket)
        {
            MultigridNode newBucket;
            points.push_back(p);
            newBucket.addPoint(points.size() - 1);
            // Add new bucket to this pyramid tree
            rootBuckets[pyVal] = newBucket;

            return true;
        }
        else
        {
            currentDim++;
            return insertIntoBucket(p, currentDim, nextBucket);
        }
    }

    template<int D>
    bool Multigrid<D>::query(const Point<D>& p)
    {
        int currentDim = 0;
        HashType pyVal = hashPoint(p, currentDim);

        MultigridNode* currentBucket = getBucketPointer(&rootBuckets, pyVal);
        currentDim++;
        while (currentBucket)
        {
            if (currentBucket->isLeaf)
            {
                for (unsigned int i = 0; (i < currentBucket->numPoints()); i++)
                {
                    if (p == points[currentBucket->pointIndices[i]])
                        return true;
                }
                return false;
            }
            else
            {
                pyVal = hashPoint(p, currentDim);
                currentBucket = getBucketPointer(
                    currentBucket->children, pyVal);
                currentDim++;
            }
        }
        return false;
    }

    template<int D>
    bool Multigrid<D>::remove(const Point<D>& p)
    {
        int currentDim = 0;
        HashType pyVal = hashPoint(p, currentDim);

        MultigridNode* currentBucket = getBucketPointer(&rootBuckets, pyVal);
        currentDim++;
        while (currentBucket)
        {
            if (currentBucket->isLeaf)
            {
                int index = 0;
                bool found = false;
                for (index; (index < currentBucket->numPoints()); index++)
                {
                    if (p == points[currentBucket->pointIndices[index]])
                    {
                        found = true;
                        break;
                    }
                }
                if (found)
                {
                    int pointIndex = currentBucket->pointIndices[index];
                    currentBucket->removePoint(pointIndex);
                    // Add index of removed point to list of unused indices
                    unusedIndices.push(pointIndex);
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {    
                currentBucket = getBucketPointer(currentBucket->children,
                    hashPoint(p, currentDim));
                currentDim++;
            }
        }
        return false;
    }

    template<int D>
    inline
    int Multigrid<D>::numPoints() const
    {
        return points.size();
    }

    template<int D>
    inline
    int Multigrid<D>::numBuckets() const
    {
        return numBuckets(rootBuckets);
    }

    template<int D>
    inline
    double Multigrid<D>::averageBucketSize() const
    {
        return points.size() / static_cast<double>(numBuckets());
    }

    template<int D>
    int Multigrid<D>::numBuckets(const Multigrid<D>::BucketMap& map) const
    {
        int total = 0;
        for (BucketMap::const_iterator it = map.begin();
            (it != map.end()); it++)
        {
            if (it->second.isLeaf)
                total += 1;
            else
                total += numBuckets(*(it->second.children));
        }
        return total;
    }

    template<int D>
    bool Multigrid<D>::insertIntoBucket(const Point<D>& p, int currentDim,
        MultigridNode* currentBucket)
    {
        if (currentBucket->isLeaf)
        {
            // Insert point into leaf since there's space!
            // (or if there aren't enough dimensions to discriminate against...)
            if (currentBucket->numPoints() < bucketSize || currentDim < D)
            {
                for (unsigned int i = 0; (i < currentBucket->numPoints()); i++)
                    if (p == points[currentBucket->pointIndices[i]])
                        return false;

                // Insert point, re-using an element for a removed point if
                // possible
                int pointIndex;
                if (!unusedIndices.empty())
                {
                    pointIndex = unusedIndices.top();
                    unusedIndices.pop();
                }
                else
                {
                    pointIndex = points.size();
                    points.push_back(p);
                }
                currentBucket->addPoint(pointIndex);
            }
            // If no more space in bucket, turn into pyramid tree with
            // (d - 1) dimensions
            else
            {
                currentBucket->children =
                    new boost::unordered_map<HashType, MultigridNode>();
                currentBucket->isLeaf = false;
                // Distribute currently stored points into the map, using the
                // correct pyramid value
                for (unsigned int i = 0; (i < currentBucket->numPoints()); i++)
                {
                    insertIntoBucket(
                        points[currentBucket->pointIndices[i]],
                        currentDim, currentBucket);
                }
                // Now all points have been inserted, clear point index array
                // for new non-leaf
                currentBucket->pointIndices.clear();
                // Now insert the input point
                insertIntoBucket(p, currentDim, currentBucket);
            }
            return true;
        }
        else // if non-leaf node - i.e. a pyramid tree!
        {
            HashType pyVal = hashPoint(p, currentDim);
            MultigridNode* nextBucket = getBucketPointer(
                currentBucket->children, pyVal);
            // If bucket which would contain point does not exist
            if (!nextBucket)
            {
                // Create new bucket and insert given point into it
                MultigridNode newBucket;
                points.push_back(p);
                newBucket.addPoint(points.size() - 1);
                // Add new bucket to this pyramid tree
                (*currentBucket->children)[pyVal] = newBucket;

                return true;
            }
            else
            {
                currentDim++;
                return insertIntoBucket(p, currentDim, nextBucket);
            }
        }
    }

    template<int D>
    inline
    Real Multigrid<D>::normaliseCoord(Real coord, Real min, Real max)
    {
        return (coord - min) / (max - min);
    }

    template<int D>
    inline
    HashType Multigrid<D>::hashPoint(const Point<D>& p, int d)
    {
        return normaliseCoord(p[d], boundary[d].min, boundary[d].max)
                 * intervalsPerDimension;
    }

    template<int D>
    inline
    MultigridNode* Multigrid<D>::getBucketPointer(
        BucketMap* map, Real hashValue)
    {
        BucketMap::iterator it = map->find(hashValue);
        if (it == map->end())
            return NULL;
        else
            return &(it->second);
    }

}