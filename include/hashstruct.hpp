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

#include "types.hpp"
#include "point.hpp"
#include <boost/unordered_map.hpp>

namespace mdsearch
{

    /** Remove element at given index from vector, using erase-remove
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

    /** A generic hash-based index structure. It hashes points to a
     * one-dimensional value and uses that value as the key into a hash ma. */
    template<int D, typename ELEM_TYPE>
    class HashStructure
    {

    public:
        /** Clear all points currently stored in the structure. */
        void clear();

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

        /** Return total number of points currently stored in the structure. */
        unsigned int numPointsStored() const;
        /** Return total number of buckets in structure. */
        unsigned int numBuckets() const;
        /** Return average number of points stored in a bucket. */
        ELEM_TYPE averagePointsPerBucket() const;
        /** Return minimum number of points stored in a single bucket. */
        unsigned int minPointsPerBucket() const;
        /** Return maximum number of points stored in a single bucket. */
        unsigned int maxPointsPerBucket() const;

    protected:
        /** Structure used to store all points with the same hash value. */
        struct Bucket
        {
            /** Stores all points in bucket. */
            std::vector< Point<D, ELEM_TYPE> > points;
            /* Vector that corresponds with 'points'. For each point, this
             * stores its summed coordinates. Used for optimisation search
             * through buckets. */
             std::vector<ELEM_TYPE> pointSums;
        };
        /** Maps 1D hash values to buckets. */
        typedef boost::unordered_map<HashType, Bucket> OneDMap;

        /** Retrieve bucket containing given point.
         * Return NULL if no bucket contains the point. */
        Bucket* getContainingBucket(const Point<D, ELEM_TYPE>& point);

        /** Get index of given point in given bucket.
         * Return -1 if point could not be found in bucket. */
        int getPointIndexInBucket(const Point<D, ELEM_TYPE>& point,
                                  const Bucket* bucket) const;

        /** Hashes point to one-dimensional value.
         * Must be implemented by suc-classes. */
        virtual HashType hashPoint(const Point<D, ELEM_TYPE>& p) = 0;

        /** Unordered_map for storing the points. Key = hashed 1D
         * representation of point, value = list of points. */
        OneDMap hashMap;

    };

    template<int D, typename ELEM_TYPE>
    inline
    void HashStructure<D, ELEM_TYPE>::clear()
    {
        // NOTE: Using assignment not clear() to ensure memory is de-allocated
        // (through destructors of containers)
        hashMap = OneDMap();
    }

    template<int D, typename ELEM_TYPE>
    inline
    bool HashStructure<D, ELEM_TYPE>::insert(const Point<D, ELEM_TYPE>& point)
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

    template<int D, typename ELEM_TYPE>
    inline
    bool HashStructure<D, ELEM_TYPE>::remove(const Point<D, ELEM_TYPE>& point)
    {
        Bucket* bucket = getContainingBucket(point);
        // Bucket has been found, point MIGHT be stored in structure
        if (bucket)
        {
            int index = getPointIndexInBucket(point, bucket);
            // If the point was found in bucket
            if (index != -1)
            {
                // Swapping last element with the element to remove, so when
                // the desired point is removed it won't cause a potentially
                // O(n) element shift operation.
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

    template<int D, typename ELEM_TYPE>
    inline
    bool HashStructure<D, ELEM_TYPE>::query(const Point<D, ELEM_TYPE>& point)
    {
        Bucket* bucket = getContainingBucket(point);
        return (bucket && (getPointIndexInBucket(point, bucket) != -1));
    }

    template<int D, typename ELEM_TYPE>
    inline
    unsigned int HashStructure<D, ELEM_TYPE>::numPointsStored() const
    {
        int total = 0;
        for (typename OneDMap::const_iterator it = hashMap.begin();
            (it != hashMap.end()); it++)
        {
            total += it->second.points.size();
        }
        return total;
    }

    template<int D, typename ELEM_TYPE>
    inline
    unsigned int HashStructure<D, ELEM_TYPE>::numBuckets() const
    {
        return hashMap.size();
    }

    template<int D, typename ELEM_TYPE>
    inline
    ELEM_TYPE HashStructure<D, ELEM_TYPE>::averagePointsPerBucket() const
    {
        return numPointsStored() / numBuckets();
    }

    template<int D, typename ELEM_TYPE>
    inline
    unsigned int HashStructure<D, ELEM_TYPE>::minPointsPerBucket() const
    {
        size_t minCount = 0;
        for (typename OneDMap::const_iterator it = hashMap.begin();
            (it != hashMap.end()); it++)
        {
            minCount = std::min(minCount, it->second.points.size());
        }
        return minCount;
    }

    template<int D, typename ELEM_TYPE>
    inline
    unsigned int HashStructure<D, ELEM_TYPE>::maxPointsPerBucket() const
    {
        size_t maxCount = 0;
        for (typename OneDMap::const_iterator it = hashMap.begin();
            (it != hashMap.end()); it++)
        {
            maxCount = std::max(maxCount, it->second.points.size());
        }
        return maxCount;
    }

    template<int D, typename ELEM_TYPE>
    inline
    typename HashStructure<D, ELEM_TYPE>::Bucket*
    HashStructure<D, ELEM_TYPE>::getContainingBucket(
        const Point<D, ELEM_TYPE>& point)
    {
        // Hash point into one-dimensional key
        HashType searchKey = hashPoint(point);
        // Search underlying structure to find point's bucket
        typename OneDMap::iterator it = hashMap.find(searchKey);
        if (it != hashMap.end())
        {
            return &(it->second); // pointer to bucket
        }
        else
        {
            return NULL;
        }
    }

    template<int D, typename ELEM_TYPE>
    inline
    int HashStructure<D, ELEM_TYPE>::getPointIndexInBucket(
        const Point<D, ELEM_TYPE>& point,
        const typename HashStructure<D, ELEM_TYPE>::Bucket* bucket) const
    {
        // Search through bucket to see if it contains the given point
        ELEM_TYPE pSum = point.sum();
        for (int index = 0; (index < bucket->points.size()); index++)
        {
            if (pSum == bucket->pointSums[index]
                && point == bucket->points[index])
            {
                return index;
            }
        }
        return -1;
    }

}

#endif