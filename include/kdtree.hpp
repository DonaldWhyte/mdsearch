/******************************************************************************

mdsearch - Lightweight C++ library implementing a collection of
           multi-dimensional search structures

File:        kdtree.hpp
Description: Implements point kd-tree index structure, as described in
             Bentley's 1975 paper "Multidimensional binary search trees used
             for associative searching".

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

#ifndef MDSEARCH_KDTREE_H
#define MDSEARCH_KDTREE_H

#include "types.hpp"
#include "point.hpp"
#include <algorithm>

namespace mdsearch
{

    template<int D, typename ELEM_TYPE>
    class KDTree
    {

    public:
        KDTree();
        ~KDTree();

        void clear();
        bool insert(const Point<D, ELEM_TYPE>& p);
        bool query(const Point<D, ELEM_TYPE>& p);
        bool remove(const Point<D, ELEM_TYPE>& p);

    private:
        struct Node
        {
            Point<D, ELEM_TYPE> point;
            Node* leftChild;
            Node* rightChild;

            Node(const Point<D, ELEM_TYPE>& p)
            : point(p), leftChild(NULL), rightChild(NULL)
            {

            }
            ~Node()
            {
                delete leftChild;
                delete rightChild;
            }
        };

        unsigned int nextCuttingDimension(unsigned int cuttingDim); 
        Node* recursiveRemove(Node* node,
                              const Point<D, ELEM_TYPE>& p,
                              unsigned int cuttingDim,
                              bool* removed);
        const Point<D, ELEM_TYPE>* findMinimum(Node* node,
                                               unsigned int dimension,
                                               unsigned int cuttingDim);

    private:
        Node* root;

    };

    template<int D, typename ELEM_TYPE>
    KDTree<D, ELEM_TYPE>::KDTree() : root(NULL)
    {
    }

    template<int D, typename ELEM_TYPE>
    KDTree<D, ELEM_TYPE>::~KDTree()
    {
        delete root;
    }

    template<int D, typename ELEM_TYPE>
    void KDTree<D, ELEM_TYPE>::clear()
    {
        delete root;
        root = NULL;
    }

    template<int D, typename ELEM_TYPE>
    bool KDTree<D, ELEM_TYPE>::insert(const Point<D, ELEM_TYPE>& p)
    {
        Node* previous = NULL; // previous node traversed
        // Set to true if 'current' is left child of 'previous'
        bool leftChildOfPrevious = false;
        Node* current = root;
        unsigned int cuttingDim = 0;

        // Loop util true/false returned - guaranteed to terminate eventually!
        while (true)
        {
            if (current == NULL)
            {
                current = new Node(p);
                // Assign parent's correct child pointer to new node
                if (previous)
                {
                    if (leftChildOfPrevious)
                        previous->leftChild = current;
                    else
                        previous->rightChild = current;
                }
                else // if no parent, then ROOT NODE WAS INSERTED. Update root!
                {
                    root = current;
                }
                return true;
            }
            else if (p[cuttingDim] < current->point[cuttingDim])
            {
                previous = current;
                current = current->leftChild;
                leftChildOfPrevious = true;
            }
            // Duplicate point, it already exists! Cannot insert point
            else if (p == current->point)
            {
                return false;
            }
            else
            {
                previous = current;
                current = current->rightChild;
                leftChildOfPrevious = false;
            }
            cuttingDim = nextCuttingDimension(cuttingDim);
        }
    }

    template<int D, typename ELEM_TYPE>
    bool KDTree<D, ELEM_TYPE>::query(const Point<D, ELEM_TYPE>& p)
    {
        Node* current = root;
        unsigned int cuttingDim = 0;
        while (current) // until end of tree is reached
        {
            if (p == current->point)
            {
                return true;
            }
            else if (p[cuttingDim] < current->point[cuttingDim])
            {
                current = current->leftChild;
            }
            else
            {
                current = current->rightChild;
            }
            cuttingDim = nextCuttingDimension(cuttingDim);
        }
        return false;
    }

    template<int D, typename ELEM_TYPE>
    bool KDTree<D, ELEM_TYPE>::remove(const Point<D, ELEM_TYPE>& p)
    {
        bool removed = false;
        root = recursiveRemove(root, p, 0, &removed);
        return removed;
    }

    template<int D, typename ELEM_TYPE>
    inline
    unsigned int KDTree<D, ELEM_TYPE>::nextCuttingDimension(
        unsigned int cuttingDim)
    {
        return (cuttingDim + 1) % D;
    }    

    template<int D, typename ELEM_TYPE>
    typename KDTree<D, ELEM_TYPE>::Node* KDTree<D, ELEM_TYPE>::recursiveRemove(
        typename KDTree<D, ELEM_TYPE>::Node* node,
        const Point<D, ELEM_TYPE>& p,
        unsigned int cuttingDim,
        bool* removed)
    {
        if (node == NULL)
        {
            return NULL;
        }
        else if (p[cuttingDim] < node->point[cuttingDim])
        {
            node->leftChild = recursiveRemove(node->leftChild, p,
                nextCuttingDimension(cuttingDim), removed);
        }
        else if (p[cuttingDim] > node->point[cuttingDim])
        {
            node->rightChild = recursiveRemove(node->rightChild, p,
                nextCuttingDimension(cuttingDim), removed);
        }
        else // found node that stores given point
        {
            // If node with point is leaf node, simply delete it!
            if (node->leftChild == NULL && node->rightChild == NULL)
            {
                // Set 'removed' flag to true to signal success
                *removed = true; 
                delete node;
                return NULL; // to remove reference to node in parent
            }
            else
            {
                // Find minimum point for cutting dimension and REPLACE node's
                // point with it
                if (node->rightChild)
                {
                    node->point = *findMinimum(node->rightChild, cuttingDim,
                        nextCuttingDimension(cuttingDim));
                    node->rightChild = recursiveRemove(
                        node->rightChild, node->point,
                        nextCuttingDimension(cuttingDim), removed);
                }
                else // if there is no right child!!
                {
                    node->point = *findMinimum(node->leftChild, cuttingDim,
                        nextCuttingDimension(cuttingDim));
                    node->leftChild = recursiveRemove(
                        node->leftChild, node->point,
                        nextCuttingDimension(cuttingDim), removed);
                    // Swap left child with right child
                    node->rightChild = node->leftChild;
                    node->leftChild = NULL;
                }
            }
        }
        // If this point is reached, node should not be removed so we
        // just return the node
        return node;
    }

    template<int D, typename ELEM_TYPE>
    const Point<D, ELEM_TYPE>* KDTree<D, ELEM_TYPE>::findMinimum(
        Node* node, unsigned int dimension, unsigned int cuttingDim)
    {
        // Reached leaf node
        if (node == NULL)
        {
            return NULL;
        }
        // If cutting dimension is dimension we're looking for minimum in,
        // just search left child!
        else if (dimension == cuttingDim)
        {
            if (node->leftChild == NULL) // if no more 
                return &node->point;
            else
                return findMinimum(node->leftChild,
                    dimension, nextCuttingDimension(cuttingDim));
        }
        // Otherwise, we have to search BOTH children
        else
        {
            const Point<D, ELEM_TYPE>* a = findMinimum(node->leftChild,
                dimension, nextCuttingDimension(cuttingDim));
            const Point<D, ELEM_TYPE>* b = findMinimum(node->rightChild,
                dimension, nextCuttingDimension(cuttingDim));
            if (a && b) // if minimums were returned from both children
            {
                Real minVal = std::min(node->point[dimension],
                    std::min((*a)[dimension], (*b)[dimension]));
                if (minVal == node->point[dimension])
                {
                    return &node->point;
                }
                else if (minVal == (*a)[dimension])
                    return a;
                else
                    return b;
            }
            else if (a) // if minimum was just returned from left child
            {
                Real minVal = std::min(
                    node->point[dimension], (*a)[dimension]);
                if (minVal == node->point[dimension])
                    return &node->point;
                else
                    return a;
            }
            else if (b) // if minimum was just returned from right child
            {
                Real minVal = std::min(
                    node->point[dimension], (*b)[dimension]);
                if (minVal == node->point[dimension])
                    return &node->point;
                else
                    return b;
            }
            else // no minimums returned!
            {
                return &node->point;
            }

        }
    }

}

#endif