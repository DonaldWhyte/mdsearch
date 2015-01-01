/******************************************************************************

mdsearch - Lightweight C++ library implementing a collection of
           multi-dimensional search structures

File:        bucket_kdtree.hpp
Description: Implements bucket kd-tree index structure. This is similar to the
             point kd-tree in that it uses a different dimension for each
             partition of the space, but how it chooses the dimension and the
             value to cut differs. It collects several points in a single node,
             and then uses those points to come up with a smarter partitioning
             strategy (rather than basing the partition on a single point,
             like the point kd-tree).

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

#ifndef MDSEARCH_BUCKET_KDTREE_H
#define MDSEARCH_BUCKET_KDTREE_H

#include "point.hpp"
#include <algorithm>
#include <cassert>

namespace mdsearch
{

namespace
    {

        template<int D, typename ELEM_TYPE>
        inline
        ELEM_TYPE rangeOfDimension(int d,
            const std::vector< Point<D, ELEM_TYPE> >& points)
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
        int dimensionWithHighestRange(
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
        ELEM_TYPE averageOfDimension(int d,
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


    /** Maximum number of points allowed in a bucket. */
    static const size_t MAX_POINTS_PER_BUCKET = 8;
    /** Minimum number of points before removing another point will force
     * the node is merge its children. */
    static const size_t MIN_POINTS_BEFORE_MERGE = MAX_POINTS_PER_BUCKET / 2;

    /* Represents single node in bucket kd-tree. */
    template<int D, typename ELEM_TYPE>
    class BucketKDTreeNode
    {

    public:
        typedef Point<D, ELEM_TYPE> PointType;
        typedef std::vector<PointType> PointList;

        /** Construct leaf node with no points. */
        BucketKDTreeNode(BucketKDTreeNode<D, ELEM_TYPE>* parent);
        /** Construct leaf node that stores given point. */
        BucketKDTreeNode(BucketKDTreeNode<D, ELEM_TYPE>* parent,
                         const PointType& p);
        /** Construct leaf node that stores given points. */
        BucketKDTreeNode(BucketKDTreeNode<D, ELEM_TYPE>* parent,
                         const PointList& points);

        /** Delete node and both of its children. */
        ~BucketKDTreeNode();

        /** Return true if node contains given point. */
        bool contains(const PointType& p);

        // ACCESSORS
        inline BucketKDTreeNode<D, ELEM_TYPE>* parent()
        {
            return m_parent;
        }
        inline int totalPoints() const { return m_totalPoints; }
        inline bool isLeaf() const { return m_isLeaf; }
        inline const PointList& points() const { return m_points; }
        inline BucketKDTreeNode<D, ELEM_TYPE>* leftChild()
        {
            return m_leftChild;
        }
        inline BucketKDTreeNode<D, ELEM_TYPE>* rightChild()
        {
            return m_rightChild;
        }
        inline int cuttingDimension() const { return m_cuttingDimension; }
        inline ELEM_TYPE cuttingValue() const { return m_cuttingValue; }

        // MUTATORS
        inline void setIsLeaf(bool leaf)
        {
            m_isLeaf = leaf;
        }

        inline void setCuttingDimension(bool cuttingDim)
        {
            m_cuttingDimension = cuttingDim;
        }

        inline void setCuttingValue(bool cuttingVal)
        {
            m_cuttingValue = cuttingVal;
        }

        /** Increment total point count on this node.
         * Recursively propagates increment by calling this method on parent
         * nodes. */
        void incrementTotalPoints();
        /** Decrement total point count on this node.
         * Recursively propagates decrement by calling this method on parent
         * nodes. */
        void decrementTotalPoints();

        /** Add point to leaf node.
         * This should only be used if node is a leaf. Calling this on a
         * non-leaf node will result in undefined behaviour. */
        bool addPoint(const PointType& p);

        /** Remove point from leaf node.
         * This should only be used if node is a leaf. Calling this on a
         * non-leaf node will result in undefined behaviour. */
        bool removePoint(const PointType& p);

    private:
        /** Splits points using a single dimension (D - 1 hyperplane). */
        class SplitPredicate
        {

        public:
            SplitPredicate(int cuttingDimension, ELEM_TYPE cuttingValue);

            /** Return true if point lies on the lower end of the cutting plane. */
            bool operator()(const Point<D, ELEM_TYPE>& p);

        private:
            /** Dimension to use when partitioning points. */
            int m_cuttingDimension;
            /** Value in cutting dimension at which points are partitioned. */
            ELEM_TYPE m_cuttingValue;

        };

        /** Split leaf node into two children and insert given point into
         * one of those children.
         * This should only be used if node is a leaf. Calling this on a
         * non-leaf node will result in undefined behaviour. */
        void splitAndInsert(const PointType& p);
        /** If node contains less than a certain number of points, such that
         * the two children MUST be leaves, then merge both children into
         * this node, making this node a leaf.
         * This should only be used if node is NOT A LEAF. Calling this on a
         * leaf node will result in undefined behaviour. */
        void attemptMerge();

        /** Pointer to parent node.
         * Set to NULL if node is the root. */
        BucketKDTreeNode<D, ELEM_TYPE>* m_parent;
        /** Total number of points store in subtree rooted at this node. */
        int m_totalPoints;

        /** Set to true if node is a leaf. */
        bool m_isLeaf;
        /** Contains all points stored in node.
         * Only used if node is a leaf. */
        PointList m_points;

        /** Pointer to left child of node.
         * NULL if node has no left child. */
        BucketKDTreeNode<D, ELEM_TYPE>* m_leftChild;
        /** Pointer to right child of node.
         * NULL if node has no right child. */
        BucketKDTreeNode<D, ELEM_TYPE>* m_rightChild;
        /** Dimension this node uses to partition space.
         * Only used if node is not a leaf. */
        int m_cuttingDimension;
        /** Value in cutting dimension at which the space is partitioned.
         * Only used if node is not a leaf. */
        ELEM_TYPE m_cuttingValue;

    };

    template<int D, typename ELEM_TYPE>
    BucketKDTreeNode<D, ELEM_TYPE>::BucketKDTreeNode(
        BucketKDTreeNode<D, ELEM_TYPE>* parent)
    : m_parent(parent), m_totalPoints(0), m_isLeaf(true),
      m_leftChild(NULL), m_rightChild(NULL),
      m_cuttingDimension(0), m_cuttingValue(0)
    {
    }

    template<int D, typename ELEM_TYPE>
    BucketKDTreeNode<D, ELEM_TYPE>::BucketKDTreeNode(
        BucketKDTreeNode<D, ELEM_TYPE>* parent,
        const Point<D, ELEM_TYPE>& p)
    : m_parent(parent), m_totalPoints(1), m_isLeaf(true),
      m_leftChild(NULL), m_rightChild(NULL),
      m_cuttingDimension(0), m_cuttingValue(0)
    {
        m_points.push_back(p);
    }

    template<int D, typename ELEM_TYPE>
    BucketKDTreeNode<D, ELEM_TYPE>::BucketKDTreeNode(
        BucketKDTreeNode<D, ELEM_TYPE>* parent,
        const PointList& points)
    : m_parent(parent), m_totalPoints(points.size()),
      m_isLeaf(true), m_points(points),
      m_leftChild(NULL), m_rightChild(NULL),
      m_cuttingDimension(0), m_cuttingValue(0)
    {

    }

    template<int D, typename ELEM_TYPE>
    BucketKDTreeNode<D, ELEM_TYPE>::~BucketKDTreeNode()
    {
        delete m_leftChild;
        delete m_rightChild;
    }

    template<int D, typename ELEM_TYPE>
    inline
    void BucketKDTreeNode<D, ELEM_TYPE>::incrementTotalPoints()
    {
        m_totalPoints++;
        if (m_parent)
        {
            m_parent->incrementTotalPoints();
        }
    }

    template<int D, typename ELEM_TYPE>
    inline
    void BucketKDTreeNode<D, ELEM_TYPE>::decrementTotalPoints()
    {
        m_totalPoints--;
        if (m_parent)
        {
            m_parent->decrementTotalPoints();
        }
    }

    template<int D, typename ELEM_TYPE>
    inline
    bool BucketKDTreeNode<D, ELEM_TYPE>::contains(
        const typename BucketKDTreeNode<D, ELEM_TYPE>::PointType& p)
    {
        typename PointList::const_iterator result = std::find(
            m_points.begin(), m_points.end(), p);
        return (result != m_points.end());
    }

    template<int D, typename ELEM_TYPE>
    inline
    bool BucketKDTreeNode<D, ELEM_TYPE>::addPoint(
        const typename BucketKDTreeNode<D, ELEM_TYPE>::PointType& p)
    {
        if (contains(p)) // if point already in structure, don't add!
        {
            return false;
        }
        else
        {
            if (m_points.size() >= MAX_POINTS_PER_BUCKET)
            {
                splitAndInsert(p);
            }
            else
            {
                m_points.push_back(p);
                incrementTotalPoints();
            }

            return true;
        }
    }

    template<int D, typename ELEM_TYPE>
    inline
    bool BucketKDTreeNode<D, ELEM_TYPE>::removePoint(
        const typename BucketKDTreeNode<D, ELEM_TYPE>::PointType& p)
    {
        if (contains(p))
        {
            m_points.erase(
                std::remove(m_points.begin(), m_points.end(), p)
            );
            decrementTotalPoints();

            // Now that point has been removed, it may be worth merging
            // siblings into single node
            if (m_parent)
            {
                m_parent->attemptMerge();
            }

            return true;
        }
        else
        {
            return false;
        }
    }

    template<int D, typename ELEM_TYPE>
    void BucketKDTreeNode<D, ELEM_TYPE>::splitAndInsert(
        const typename BucketKDTreeNode<D, ELEM_TYPE>::PointType& p)
    {
        // CUTTING DIMENSION: Use dimension with highest range of values
        // CUTTING VALUE: Use average value of dth coordinate of node's points
        int cuttingDimension = dimensionWithHighestRange(m_points);
        ELEM_TYPE cuttingValue = averageOfDimension(
            cuttingDimension, m_points);

        // Partition points using cutting plane
        SplitPredicate predicate(cuttingDimension, cuttingValue);
        typename PointList::iterator endOfLeft = std::partition(
            m_points.begin(), m_points.end(), predicate);

        // Construct children to hold both partitions
        m_leftChild = new BucketKDTreeNode<D, ELEM_TYPE>(
            this, PointList(m_points.begin(), endOfLeft)
        );
        m_rightChild = new BucketKDTreeNode<D, ELEM_TYPE>(
            this, PointList(endOfLeft, m_points.end())
        );

        // Make given node a non-leaf
        m_isLeaf = false;
        m_points.clear();
        m_cuttingDimension = cuttingDimension;
        m_cuttingValue = cuttingValue;

        // Insert given point into one of the new children
        if (p[m_cuttingDimension] < m_cuttingValue)
        {
            m_leftChild->addPoint(p);
        }
        else
        {
            m_rightChild->addPoint(p);
        }
    }

    template<int D, typename ELEM_TYPE>
    inline
    void BucketKDTreeNode<D, ELEM_TYPE>::attemptMerge()
    {
        // If children of this node contain less than a certain number
        // of points, merge the two children together and make this node
        // a leaf
        if (m_totalPoints < MIN_POINTS_BEFORE_MERGE)
        {
            m_points = m_leftChild->points();

            m_points.insert(m_points.end(),
                m_rightChild->points().begin(),
                m_rightChild->points().end());


            m_isLeaf = true;
            delete m_leftChild;
            delete m_rightChild;
            m_leftChild = NULL;
            m_rightChild = NULL;

            if (m_parent)
            {
                m_parent->attemptMerge();
            }
        }
    }

    template<int D, typename ELEM_TYPE>
    BucketKDTreeNode<D, ELEM_TYPE>::SplitPredicate::SplitPredicate(
        int cuttingDimension, ELEM_TYPE cuttingValue)
    : m_cuttingDimension(cuttingDimension), m_cuttingValue(cuttingValue)
    {
    }

    template<int D, typename ELEM_TYPE>
    bool BucketKDTreeNode<D, ELEM_TYPE>::SplitPredicate::operator()(
        const Point<D, ELEM_TYPE>& p)
    {
        return (p[m_cuttingDimension] < m_cuttingValue);
    }

    /** Implements bucket kd-tree index structure. Unlike the point kd-tree,
     * each node of the structure stores several points. When the capacity of
     * a node is filled, it is split into two children nodes.
     *
     * Nodes are only stored in the leaves of the tree in this structure.
    */
    template<int D, typename ELEM_TYPE>
    class BucketKDTree
    {

    public:
        /** Construct empty bucket kd-tree. */
        BucketKDTree();
        /** Delete root node of tree and all of its children. */
        ~BucketKDTree();

        /** Remove all points from tree. */
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

        /** Return total number of points stored in structure. */
        int totalPoints() const;

    private:
        typedef BucketKDTreeNode<D, ELEM_TYPE> NodeType;

        /** Find lead node that corresponds to spatial region that contains
         * given point. */
        NodeType* findLeafFor(const Point<D, ELEM_TYPE>& p);

        /** Root node of tree. */
        NodeType* m_root;

    };

    template<int D, typename ELEM_TYPE>
    BucketKDTree<D, ELEM_TYPE>::BucketKDTree()
    : m_root(new NodeType(NULL))
    {
    }

    template<int D, typename ELEM_TYPE>
    BucketKDTree<D, ELEM_TYPE>::~BucketKDTree()
    {
        delete m_root;
    }

    template<int D, typename ELEM_TYPE>
    void BucketKDTree<D, ELEM_TYPE>::clear()
    {
        delete m_root;
        m_root = new NodeType(NULL);
    }

    template<int D, typename ELEM_TYPE>
    bool BucketKDTree<D, ELEM_TYPE>::insert(const Point<D, ELEM_TYPE>& p)
    {
        NodeType* leaf = findLeafFor(p);
        return (leaf && leaf->addPoint(p));
    }

    template<int D, typename ELEM_TYPE>
    bool BucketKDTree<D, ELEM_TYPE>::query(const Point<D, ELEM_TYPE>& p)
    {
        NodeType* leaf = findLeafFor(p);
        return (leaf && leaf->contains(p));
    }

    template<int D, typename ELEM_TYPE>
    bool BucketKDTree<D, ELEM_TYPE>::remove(const Point<D, ELEM_TYPE>& p)
    {
        NodeType* leaf = findLeafFor(p);
        return (leaf && leaf->removePoint(p));
    }

    template<int D, typename ELEM_TYPE>
    inline
    int BucketKDTree<D, ELEM_TYPE>::totalPoints() const
    {
        return m_root->totalPoints();
    }

    template<int D, typename ELEM_TYPE>
    typename BucketKDTree<D, ELEM_TYPE>::NodeType*
    BucketKDTree<D, ELEM_TYPE>::findLeafFor(const Point<D, ELEM_TYPE>& p)
    {
        NodeType* current = m_root;
        while (!current->isLeaf())
        {
            // Ensure non-leaf node has two children (should always be case)
            assert(current->leftChild() && current->rightChild());

            if (p[current->cuttingDimension()] < current->cuttingValue())
            {
                current = current->leftChild();
            }
            else
            {
                current = current->rightChild();
            }
        }

        return current;
    }

}

#endif