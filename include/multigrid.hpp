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

#ifndef MDSEARCH_MULTIGRID_H
#define MDSEARCH_MULTIGRID_H

#include "point.hpp"
#include "boundary.hpp"
#include <stack>
#include <boost/unordered_map.hpp>
#include <algorithm>

namespace mdsearch
{

    struct MultigridNode
    {
        /** Construct empty leaf node. */
        MultigridNode();
        /** Recursively delete child nodes. */
        ~MultigridNode();

        /** Add point to node. If node is a leaf and it has enough space,
         * the point will be stored. If the node is not a leaf, it will
         * hash the point and store it inside one of its children. */
        void addPoint(int pointIndex);

        /** Remove point with given index from node. If node is not a leaf,
         * it will try and remove the node from its children.
         * Return true if point was removed and false if point was not stored
         * in this node. */
        bool removePoint(int pointIndex);

        /** Return number of points stored DIRECTLY in this node. Contents of
         * child nodes are ignored. Hence, this will always return 0 if node
         * is not a leaf. */
        std::size_t numPoints() const;

        /** True if node is a leaf, and stores points. */
        bool isLeaf;
        /** Indices of points contained in bucket.
         * Only used if node is a leaf. */
        std::vector<int> pointIndices;

        /** Indices of points contained in bucket.
         * Only used if node is a non-leaf. */
        boost::unordered_map<HashType, MultigridNode>* children;

    };

    /**
     * The Multigrid Tree decomposes the data space into a uniform grid
     * by cutting each dimension into B intervals. A cell is defined by
     * the interval of each dimension it is contained in, meaning there
     * are a total of B^d cells, where d is the number of dimensions.
    */
    template<int D, typename ELEM_TYPE>
    class Multigrid
    {

    public:
        /** Construct Multigrid Tree to cover given boundary.
         *
         * \param m_intervalsPerDimension determines how many buckets will be
         * used for each dimension. More buckets means more discrimination
         * and less points in each bucket, on average.
         *
         * \param m_bucketSize determines initial amount of memory a bucket
         * reserves to store points.
        */
        Multigrid(const Boundary<D, ELEM_TYPE>& boundary,
            ELEM_TYPE m_intervalsPerDimension = 1000000000,
            int m_bucketSize = 8);

        /** Clear all points in Multigrid Tree and reset its spatial
         * boundary. */
        void clear(const Boundary<D, ELEM_TYPE>& newBoundary);

        /** Insert point into structure.
         * Returns true if the point was inserted successfully and
         * false if the point is already stored in the structure. */
        bool insert(const Point<D, ELEM_TYPE>& point);

        /** Remove point from the structure.
         * Returns true if the point was removed successfully and
         * false if the point was not being stored. */
        bool remove(const Point<D, ELEM_TYPE>& point);

        /** Return true if the given point is being stored in the structure. */
        bool query(const Point<D, ELEM_TYPE>& point);

        /** Return total number of points stored. */
        int numPoints() const;
        /** Return total number of buckets stored. */
        int numBuckets() const;
        /** Return average number of points in each bucket. */
        double averageBucketSize() const;

    private:
        /** Maps 1D point hash values into Multigrid Tree nodes. */
        typedef boost::unordered_map<HashType, MultigridNode> BucketMap;

        /** Return total number of buckets in given map,by recursively
         * searching through it. */
        int numBuckets(const BucketMap& map) const;
        /** Insert point into given bucket. The given dimension is used to hash
         * the point. */
        bool insertIntoBucket(const Point<D, ELEM_TYPE>& p,
                              int currentDim,
                              MultigridNode* currentBucket);
        /** Normalise value into 0-1 range based on min-max interval. */
        ELEM_TYPE normaliseCoord(ELEM_TYPE coord, ELEM_TYPE min, ELEM_TYPE max);
        /** Hash point using the vale of its dth coordinate. */
        HashType hashPoint(const Point<D, ELEM_TYPE>& p, int d);
        /** Retrieve pointer to bucket that contains points that have the
         * given hash value. */
        MultigridNode* getBucketPointer(BucketMap* map, ELEM_TYPE hashValue);


        /** Spatial boundary covered by Multigrid Tree. */
        Boundary<D, ELEM_TYPE> boundary;
        /** Determines how many buckets will be used for each dimension.
         * More buckets means more discrimination and less points in each
         * bucket, on average. */
        ELEM_TYPE m_intervalsPerDimension;
        /** Determines initial amount of memory a bucket reserves to store
         * points. */
        int m_bucketSize;
        /** Stores root Multigrid Tree nodes. These are accessed by hashing the
         * value of a point's FIRST coordinate. */
        BucketMap m_rootBuckets;
        /** Contains all points stored in tree. */
        std::vector< Point<D, ELEM_TYPE> > m_points;
        /** Marks elements in 'points' vector as being unused. These will be
         * re-used when points are inserted in the future. */
        std::stack<int> m_unusedIndices;

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

    template<int D, typename ELEM_TYPE>
    Multigrid<D, ELEM_TYPE>::Multigrid(const Boundary<D, ELEM_TYPE>& boundary,
        ELEM_TYPE m_intervalsPerDimension, int m_bucketSize) :
        boundary(boundary),
        m_intervalsPerDimension(m_intervalsPerDimension),
        m_bucketSize(m_bucketSize)
    {
    }

    template<int D, typename ELEM_TYPE>
    inline
    void Multigrid<D, ELEM_TYPE>::clear(
        const Boundary<D, ELEM_TYPE>& newBoundary)
    {
        boundary = newBoundary;
        m_rootBuckets = BucketMap();
    }

    template<int D, typename ELEM_TYPE>
    bool Multigrid<D, ELEM_TYPE>::insert(const Point<D, ELEM_TYPE>& p)
    {
        int currentDim = 0;
        HashType pyVal = hashPoint(p, currentDim);

        // Find the next bucket to traverse
        MultigridNode* nextBucket = getBucketPointer(&m_rootBuckets, pyVal);
        // If bucket not found, create new bucket and insert point into it
        if (!nextBucket)
        {
            MultigridNode newBucket;
            m_points.push_back(p);
            newBucket.addPoint(m_points.size() - 1);
            // Add new bucket to this pyramid tree
            m_rootBuckets[pyVal] = newBucket;

            return true;
        }
        else
        {
            currentDim++;
            return insertIntoBucket(p, currentDim, nextBucket);
        }
    }

    template<int D, typename ELEM_TYPE>
    bool Multigrid<D, ELEM_TYPE>::query(const Point<D, ELEM_TYPE>& p)
    {
        int currentDim = 0;
        HashType pyVal = hashPoint(p, currentDim);

        MultigridNode* currentBucket = getBucketPointer(&m_rootBuckets, pyVal);
        currentDim++;
        while (currentBucket)
        {
            if (currentBucket->isLeaf)
            {
                for (unsigned int i = 0; (i < currentBucket->numPoints()); i++)
                {
                    if (p == m_points[currentBucket->pointIndices[i]])
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

    template<int D, typename ELEM_TYPE>
    bool Multigrid<D, ELEM_TYPE>::remove(const Point<D, ELEM_TYPE>& p)
    {
        int currentDim = 0;
        HashType pyVal = hashPoint(p, currentDim);

        MultigridNode* currentBucket = getBucketPointer(&m_rootBuckets, pyVal);
        currentDim++;
        while (currentBucket)
        {
            if (currentBucket->isLeaf)
            {
                int index = 0;
                bool found = false;
                for (index; (index < currentBucket->numPoints()); index++)
                {
                    if (p == m_points[currentBucket->pointIndices[index]])
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
                    m_unusedIndices.push(pointIndex);
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

    template<int D, typename ELEM_TYPE>
    inline
    int Multigrid<D, ELEM_TYPE>::numPoints() const
    {
        return m_points.size();
    }

    template<int D, typename ELEM_TYPE>
    inline
    int Multigrid<D, ELEM_TYPE>::numBuckets() const
    {
        return numBuckets(m_rootBuckets);
    }

    template<int D, typename ELEM_TYPE>
    inline
    double Multigrid<D, ELEM_TYPE>::averageBucketSize() const
    {
        return m_points.size() / static_cast<double>(numBuckets());
    }

    template<int D, typename ELEM_TYPE>
    int Multigrid<D, ELEM_TYPE>::numBuckets(
        const Multigrid<D, ELEM_TYPE>::BucketMap& map) const
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

    template<int D, typename ELEM_TYPE>
    bool Multigrid<D, ELEM_TYPE>::insertIntoBucket(
        const Point<D, ELEM_TYPE>& p,
        int currentDim,
        MultigridNode* currentBucket)
    {
        if (currentBucket->isLeaf)
        {
            // Insert point into leaf since there's space!
            // (or if there aren't enough dimensions to discriminate against)
            if (currentBucket->numPoints() < m_bucketSize || currentDim < D)
            {
                for (unsigned int i = 0; (i < currentBucket->numPoints()); i++)
                    if (p == m_points[currentBucket->pointIndices[i]])
                        return false;

                // Insert point, re-using an element for a removed point if
                // possible
                int pointIndex;
                if (!m_unusedIndices.empty())
                {
                    pointIndex = m_unusedIndices.top();
                    m_unusedIndices.pop();
                }
                else
                {
                    pointIndex = m_points.size();
                    m_points.push_back(p);
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
                        m_points[currentBucket->pointIndices[i]],
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
                m_points.push_back(p);
                newBucket.addPoint(m_points.size() - 1);
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

    template<int D, typename ELEM_TYPE>
    inline
    ELEM_TYPE Multigrid<D, ELEM_TYPE>::normaliseCoord(ELEM_TYPE coord,
                                                      ELEM_TYPE min,
                                                      ELEM_TYPE max)
    {
        return (coord - min) / (max - min);
    }

    template<int D, typename ELEM_TYPE>
    inline
    HashType Multigrid<D, ELEM_TYPE>::hashPoint(const Point<D, ELEM_TYPE>& p,
                                                int d)
    {
        return normaliseCoord(p[d], boundary[d].min, boundary[d].max)
                 * m_intervalsPerDimension;
    }

    template<int D, typename ELEM_TYPE>
    inline
    MultigridNode* Multigrid<D, ELEM_TYPE>::getBucketPointer(
        BucketMap* map, ELEM_TYPE hashValue)
    {
        BucketMap::iterator it = map->find(hashValue);
        if (it == map->end())
            return NULL;
        else
            return &(it->second);
    }

}

#endif
