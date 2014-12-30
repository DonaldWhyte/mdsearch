/******************************************************************************

mdsearch - Lightweight C++ library implementing a collection of
           multi-dimensional search structures

File:        test_core.cpp
Description: Test executable that runs some basic tests on the core data types
             used throughout mdsearch.

             This includes points, boundaries, float comparison and timing
             mechanisms.

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
#include "timing.hpp"
#include <iostream>

using namespace mdsearch;

void testCoreTypes()
{
    // Test point and boundary initialisation
    Real values[] = { 0.0f, 1.0f, 2.0f };
    Point<3> point(values);
    Interval intervals[3] = { Interval(0.0f, 1.0f), Interval(0.0f, 2.0f), Interval(-1.0f, 1.0f) };
    Boundary<3> boundary(&intervals[0]);

    std::cout << point << " " << point.sum() << std::endl;
    std::cout << boundary << std::endl;
}

void testFloatComparison()
{
    std::cout << "0 == 0 -> " << compare(0.0f, 0.0f) << std::endl;
    std::cout << "1 > 0 -> " << compare(1.0f, 0.0f) << std::endl;
    std::cout << "-1 < 0 -> " << compare(-1.0f, 0.0f) << std::endl;    
}

void testTiming()
{
    double startTime = getTime();
    sleep(1);
    std::cout << "Slept for " << getTime() - startTime << " seconds" << std::endl;
}

int main(int argc, char* argv[])
{
    testCoreTypes();
    testFloatComparison();
    testTiming();

    return 0;
}