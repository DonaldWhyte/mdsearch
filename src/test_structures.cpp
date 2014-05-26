#include <iostream>
#include "types.hpp"
#include "util.hpp"

using namespace mdsearch;

int main(int argc, char* argv[])
{
	Real values[] = { 0.0f, 1.0f, 2.0f };
	Point<3> point(values);

	Interval intervals[3] = { Interval(0.0f, 1.0f), Interval(0.0f, 2.0f), Interval(-1.0f, 1.0f) };
	Boundary<3> boundary(&intervals[0]);

	std::cout << point << " " << point.sum() << std::endl;
	std::cout << boundary << std::endl;

	std::cout << "0 == 0 -> " << compare(0, 0) << std::endl;
	std::cout << "1 > 0 -> " << compare(1, 0) << std::endl;
	std::cout << "-1 < 0 -> " << compare(-1, 0) << std::endl;

	return 0;
}