#include "Gomoku.hpp"

#include <iostream>

int	main(int ac, char **av)
{
	(void)ac;(void)av;

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
