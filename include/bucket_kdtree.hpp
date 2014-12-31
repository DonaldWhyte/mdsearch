/******************************************************************************

mdsearch - Lightweight C++ library implementing a collection of
           multi-dimensional search structures

File:        bucket_kdtree.hpp
Description: TODO

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

namespace mdsearch
{

    /** TODO */
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

    private:
        typedef std::vector< Point<D, ELEM_TYPE> > PointList;

        /* Represents single node in bucket kd-tree. */
        struct Node
        {
            /** Set to true if node is a leaf. */
            bool isLeaf;
            /** Contains all points stored in node.
             * Only used if node is a leaf. */
            PointList points;
            /** Pointer to left child of node.
             * NULL if node has no left child. */
            Node* leftChild;
            /** Pointer to right child of node.
             * NULL if node has no right child. */
            Node* rightChild;

            /** Construct leaf node that stores given points */
            Node(const PointList& points)
            : points(points), leftChild(NULL), rightChild(NULL)
            {

            }

            /** Delete node and both of its children. */
            ~Node()
            {
                delete leftChild;
                delete rightChild;
            }
        };

    private:
        /** Root node of tree. */
        Node* root;

    };

    template<int D, typename ELEM_TYPE>
    BucketKDTree<D, ELEM_TYPE>::BucketKDTree() : root(NULL)
    {
    }

    template<int D, typename ELEM_TYPE>
    BucketKDTree<D, ELEM_TYPE>::~BucketKDTree()
    {
        delete root;
    }

    template<int D, typename ELEM_TYPE>
    void BucketKDTree<D, ELEM_TYPE>::clear()
    {
        delete root;
        root = NULL;
    }

    template<int D, typename ELEM_TYPE>
    bool BucketKDTree<D, ELEM_TYPE>::insert(const Point<D, ELEM_TYPE>& p)
    {
        // TODO
        return false;
    }

    template<int D, typename ELEM_TYPE>
    bool BucketKDTree<D, ELEM_TYPE>::query(const Point<D, ELEM_TYPE>& p)
    {
        // TODO
        return false;
    }

    template<int D, typename ELEM_TYPE>
    bool BucketKDTree<D, ELEM_TYPE>::remove(const Point<D, ELEM_TYPE>& p)
    {
        // TODO
        return false;
    }

}

#endif