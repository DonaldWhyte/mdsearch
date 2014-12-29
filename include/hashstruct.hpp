/******************************************************************************

mdsearch - Lightweight C++ library implementing a collection of
           multi-dimensional search structures

File:        hashstruct.hpp
Description: Defines a generic hash-based index structure. It hashes points to
             a one-dimensional value and uses that value as the key into a hash
             map.

             Exactly how points are hashed is not defined. This must be
             provided by creating subclasses of HashStructure which implement
             the abstract method hashPoint().

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

#ifndef MDSEARCH_HASHSTRUCT_H
#define MDSEARCH_HASHSTRUCT_H

#include <boost/unordered_map.hpp>
#include "types.hpp"

namespace mdsearch
{

    /* Remove element at given index from vector, using erase-remove
     * idiom to prevent $(n) move operation. 
     *
     * NOTE: This deletes an element, but does NOT preserve the
     * vector's order.
     *
     * NOTE: This does NOT perform a check to ensure the given index
     * is within the bounds of the vector -- this be done by the calling
     * code. */
    template <typename T>
    inline void removeElementAtIndex(std::vector<T>& vec, unsigned int index)
    {
        std::iter_swap(vec.begin() + index, vec.end() - 1);
        vec.erase(vec.end() - 1);
    }

    template<int D>
    class HashStructure
    {

    public:
        /* Clear all points currently stored in the structure. */
        inline void clear()
        {
            // NOTE: Using assignment not clear() to ensure memory is de-allocated
            // (through destructors of containers)
            hashMap = OneDMap();
        }

        /* Insert point into the Pyramid Tree.
         * Returns true if the point was inserted successfully and
         * false if the point is already stored in the structure. */
        inline bool insert(const Point<D>& point)
        {
            // Retrieve containing bucket by hashing point into key
            HashType searchKey = hashPoint(point);
            // Search underlying 1D structure to find point's bucket
            Bucket* bucket = NULL;
            typename OneDMap::iterator it = hashMap.find(searchKey);
            if (it != hashMap.end())
                bucket = &(it->second);

            if (bucket) // if bucket for point exists
            {
                // If point is stored in bucket - it ALREADY EXISTS
                if (getPointIndexInBucket(point, bucket) != -1)
                {
                    return false;
                }
                else
                {
                    bucket->points.push_back(point);
                    bucket->pointSums.push_back(point.sum());
                    return true;
                }
            }
            else // if bucket does not exist for point, create it!
            {
                Bucket newBucket;
                newBucket.points.push_back(point);
                newBucket.pointSums.push_back(point.sum());
                hashMap[searchKey] = newBucket;
                return true;
            }
        }

        /* Remove point from the tree.
         * Returns true if the point was removed successfully and
         * false if the point was not being stored. */
        inline bool remove(const Point<D>& point)
        {
            Bucket* bucket = getContainingBucket(point);
            // Bucket has been found, point MIGHT be stored in structure
            if (bucket)
            {
                int index = getPointIndexInBucket(point, bucket);
                // If the point was found in bucket
                if (index != -1)
                {
                    // Swapping last element with the element to remove, so when the
                    // desired point is removed it won't cause a potentially O(n) move
                    // operation.
                    removeElementAtIndex(bucket->points, index);
                    removeElementAtIndex(bucket->pointSums, index);
                    return true;
                }
                // Point is not contained in bucket -- cannot remove
                else
                {
                    return false;
                }
            }
            // No bucket found, so point is not being stored in the structure
            else
            {
                return false;
            }            
        }

        /* Return true if the given point is being stored in the structure. */
        inline bool query(const Point<D>& point)
        {
            Bucket* bucket = getContainingBucket(point);
            return (bucket && (getPointIndexInBucket(point, bucket) != -1));
        }

        /* Return total number of points currently stored in the structure. */
        inline unsigned int numPointsStored() const
        {
            int total = 0;
            for (typename OneDMap::const_iterator it = hashMap.begin(); (it != hashMap.end()); it++)
                total += it->second.points.size();
            return total;
        }
        /* Return total number of buckets in structure. */
        inline unsigned int numBuckets() const
        {
            return hashMap.size();
        }
        /* Return average number of points stored in a bucket. */
        inline Real averagePointsPerBucket() const
        {
            return numPointsStored() / numBuckets();
        }
        /* Return minimum and maximum number of points stored in a single bucket. */
        inline unsigned int minPointsPerBucket() const
        {
            size_t minCount = 0;
            for (typename OneDMap::const_iterator it = hashMap.begin(); (it != hashMap.end()); it++)
                minCount = std::min(minCount, it->second.points.size());
            return minCount;
        }
        inline unsigned int maxPointsPerBucket() const
        {
            size_t maxCount = 0;
            for (typename OneDMap::const_iterator it = hashMap.begin(); (it != hashMap.end()); it++)
                maxCount = std::max(maxCount, it->second.points.size());
            return maxCount;
        }



    protected:
        struct Bucket
        {
            std::vector< Point<D> > points;
            RealList pointSums;
        };
        typedef boost::unordered_map<HashType, Bucket> OneDMap;



        inline Bucket* getContainingBucket(const Point<D>& point)
        {
            // Hash point into one-dimensional key
            HashType searchKey = hashPoint(point);
            // Search underlying structure to find point's bucket
            typename OneDMap::iterator it = hashMap.find(searchKey);
            if (it != hashMap.end())
                return &(it->second); // pointer to bucket
            else
                return NULL;
        }

        inline int getPointIndexInBucket(const Point<D>& point, const Bucket* bucket) const
        {
            // Search through points in bucket to see if it contains the given point
            Real pSum = point.sum();
            for (int index = 0; (index < bucket->points.size()); index++)
                if (pSum == bucket->pointSums[index] && point == bucket->points[index])
                    return index;
            return -1;
        }

        /* Must be implemented by subclasses. */
        virtual HashType hashPoint(const Point<D>& p) = 0;



        // Unordered_map for storing the points
        // Key = hashed 1D representation of point
        // Value = list of points in BUCKET
        OneDMap hashMap;

    };

}

#endif