#include "PmergeMe.hpp"
#include <iostream>
#include <exception>

int main(int argc, char **argv)
{
	PmergeMe sorter;
	try
	{
		sorter.run(argc, argv);
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}