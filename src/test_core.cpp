#include <iostream>
#include "types.hpp"
#include "util.hpp"
#include "timing.hpp"

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