#include "Gomoku.hpp"

#include <iostream>
#include <cmath>

int main()
{
	Gomoku	gmk;

	try
	{
		gmk.run();
	} catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}
	return (0);
}
