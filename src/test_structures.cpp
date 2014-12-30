/******************************************************************************

mdsearch - Lightweight C++ library implementing a collection of
           multi-dimensional search structures

File:        test_structures.cpp
Description: Test executable that generates a random point dataset and uses
             it to test the correctness of all the index structures implemented
             in mdsearch.

             In addition to correctness tests, this executable also runs the
             structures through some basic timing tests. It outputs the total
             time it takes the structures to insert, delete or find each point
             in the randomly generated dataset.

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

#include "dataset.hpp"
#include "timing.hpp"
#include "kdtree.hpp"
#include "multigrid.hpp"
#include "bithash.hpp"
#include "pyramidtree.hpp"
#include <iostream>

using namespace mdsearch;

// Test parameters
static const int NUM_DIMENSIONS = 10;
static const int NUM_TEST_POINTS = 100000;
typedef std::vector< Point<NUM_DIMENSIONS> > PointList;

/* Functions used to generate random test dataset. */
Real generateRandomNumber(Real minimum, Real maximum)
{
    return minimum + static_cast<float>(rand()) / static_cast<float>(RAND_MAX / (maximum - minimum));
}

PointList generateRandomPoints(unsigned int numPoints)
{
    std::vector< Point<NUM_DIMENSIONS> > points;
    Real test = generateRandomNumber(0.0f, 1.0f);
    for (unsigned int i = 0; (i < numPoints); i++)
    {
        Point<NUM_DIMENSIONS> p;
        for (unsigned int d = 0; (d < NUM_DIMENSIONS); d++)
        {
            p[d] = generateRandomNumber(0.0f, 1.0f);
        }
        points.push_back(p);
    }
    return points;
}

template<typename STRUCT_TYPE>
bool testStructureOperations(STRUCT_TYPE* structure, const PointList& points)
{
    // NOTE: Tests assume all given points are UNIQUE!!!

    // Ensure structure is entirely empty
    for (unsigned int i = 0; (i < points.size()); i++)
    {
        if (structure->query(points[i]))
        {
            std::cout << "False positive point query with point " << i << ": " << points[i] << std::endl;
            return false;
        }
    }
    // Test insertions
    for (unsigned int i = 0; (i < points.size()); i++)
    {
        // Don't test if true returned -- may be duplicates in dataset!
        structure->insert(points[i]);
    }
    // Test queries
    for (unsigned int i = 0; (i < points.size()); i++)
    {
        if (!structure->query(points[i]))
        {
            std::cout << "Failed query with point " << i << ": " << points[i] << std::endl;
            return false;
        }
    }
    // Test removals
    for (unsigned int i = 0; (i < points.size()); i++)
    {
        if (structure->remove(points[i]))
        {
            if (structure->query(points[i]))
            {
                std::cout << "Failed removal (never actually removed) with point" << i << ": " << points[i] << std::endl;    
                return false;
            }
        }
        else
        {
            std::cout << "Failed removal (returned failure signal) with point " << i << ": " << points[i] << std::endl;
            return false;
        }
    }

    return true; // all tests passed
}

template<typename STRUCT_TYPE>
void testStructure(const std::string& structureName, STRUCT_TYPE* structure, const PointList& points)
{
    std::cout << "TESTING " << structureName << "..." << std::endl;
    if (testStructureOperations<STRUCT_TYPE>(structure, points))
        std::cout << "...SUCCESS." << std::endl;
    else
        std::cout << "...FAILED." << std::endl;
}

template<typename STRUCT_TYPE>
void timeStructure(const std::string& structureName, STRUCT_TYPE* structure, const PointList& points)
{
    static const double MAX_EXECUTION_TIME = 1800.0; // in seconds
    static const int opsBetweenChecks = 300; // in # of operations

    std::cout << "TIMING " << structureName << "..." << std::endl;

    double start = getTime();
    for (unsigned int i = 0; (i < points.size()); i++)
    {
        structure->insert(points[i]);
        if ((i % opsBetweenChecks) == 0)
        {
            if ((getTime() - start) > MAX_EXECUTION_TIME)
            {
                std::cout << "\tAborted operation at point " << (i + 1) << std::endl;
                break;
            }
        }
    }
    std::cout << "\tInsertion took " << getTime() - start << " seconds" << std::endl;

    start = getTime();
    for (unsigned int i = 0; (i < points.size()); i++)
    {
        structure->query(points[i]);
        if ((i % opsBetweenChecks) == 0)
        {
            if ((getTime() - start) > MAX_EXECUTION_TIME)
            {
                std::cout << "\tAborted operation at point " << (i + 1) << std::endl;
                break;
            }
        }
    }
    std::cout << "\tQueries took " << getTime() - start << " seconds" << std::endl;
    
    start = getTime();
    for (unsigned int i = 0; (i < points.size()); i++)
    {
        structure->remove(points[i]);
        if ((i % opsBetweenChecks) == 0)
        {
            if ((getTime() - start) > MAX_EXECUTION_TIME)
            {
                std::cout << "\tAborted operation at point " << (i + 1) << std::endl;
                break;
            }
        }
    }
    std::cout << "\tDeletion took " << getTime() - start << " seconds" << std::endl;

    std::cout << "...DONE." << std::endl;
}

void testCorrectness(const PointList& points, const Boundary<NUM_DIMENSIONS>& boundary)
{
    std::cout << "-----------------CORRECTNESS TESTS-----------------" << std::endl;
    KDTree<NUM_DIMENSIONS> kdTree;
    testStructure< KDTree<NUM_DIMENSIONS> >("kd-tree", &kdTree, points);
    Multigrid<NUM_DIMENSIONS> multigrid(boundary);
    testStructure< Multigrid<NUM_DIMENSIONS> >("multigrid", &multigrid, points);
    BitHash<NUM_DIMENSIONS> bitHash;
    testStructure< BitHash<NUM_DIMENSIONS> >("bithash", &bitHash, points);
    PyramidTree<NUM_DIMENSIONS> pyramidTree(boundary);
    testStructure< PyramidTree<NUM_DIMENSIONS> >("pyramid_tree", &pyramidTree, points);
}

void testPerformance(const PointList& points, const Boundary<NUM_DIMENSIONS>& boundary)
{
    std::cout << "-----------------PERFORMANCE TESTS-----------------" << std::endl;
    KDTree<NUM_DIMENSIONS> kdTree;
    timeStructure< KDTree<NUM_DIMENSIONS> >("kd-tree", &kdTree, points);
    Multigrid<NUM_DIMENSIONS> multigrid(boundary);
    timeStructure< Multigrid<NUM_DIMENSIONS> >("multigrid", &multigrid, points);
    BitHash<NUM_DIMENSIONS> bitHash;
    timeStructure< BitHash<NUM_DIMENSIONS> >("bithash", &bitHash, points);
    PyramidTree<NUM_DIMENSIONS> pyramidTree(boundary);
    timeStructure< PyramidTree<NUM_DIMENSIONS> >("pyramid_tree", &pyramidTree, points);
}


int main(int argc, char* argv[])
{
    // Generate test data using random number generator
    srand(time(NULL)); // seed generator to get different points every time!!
    Dataset<NUM_DIMENSIONS> dataset;
    dataset.load( generateRandomPoints(NUM_TEST_POINTS) );
    Boundary<NUM_DIMENSIONS> boundary = dataset.computeBoundary();

    testCorrectness(dataset.getPoints(), boundary);
    std::cout << std::endl;
    testPerformance(dataset.getPoints(), boundary);

    return 0;
}