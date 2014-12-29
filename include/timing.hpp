/******************************************************************************

mdsearch - Lightweight C++ library implementing a collection of
           multi-dimensional search structures

File:        timing.hpp
Description: Defines functionality for computing execution time.

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

#ifndef MDSEARCH_TIMING_H
#define MDSEARCH_TIMING_H

// Interval timer used depends on platform
#if defined(_WIN32)
    #include <time.h>
#elif defined(__unix__)
    #include <sys/time.h>
#else
    #error No timing mechanism supported for this platform
#endif

namespace mdsearch
{

    /* Return current time in seconds. */
    double getTime()
    {
    #if defined(_WIN32)
        return static_cast<double>(time(NULL)); // in seconds already
    #elif defined(__unix__)
        timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts); 
        return static_cast<double>(ts.tv_sec) + (static_cast<double>(ts.tv_nsec) / 1000000000.0);
    #else
        return 0.0;
    #endif
    }
}

#endif