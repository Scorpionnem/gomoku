#include "Gomoku.hpp"

#include <iostream>
#include <cmath>

int main(int ac, char **av)
{
	Gomoku	gmk;

	Gomoku::PlayerType	p1_type = Gomoku::HUMANPLAYER;
	Gomoku::PlayerType	p2_type = Gomoku::AIPLAYER;

	int depth = 10;
	if (ac >= 3)
	{
		if (std::string(av[1]) == "ai")
			p1_type = Gomoku::AIPLAYER;
		else if (std::string(av[1]) == "human")
			p1_type = Gomoku::HUMANPLAYER;
		else
			return (1);

		if (std::string(av[2]) == "ai")
			p2_type = Gomoku::AIPLAYER;
		else if (std::string(av[2]) == "human")
			p2_type = Gomoku::HUMANPLAYER;
		else
			return (1);
	}
	if (ac == 4) depth = std::stoi(av[3]);
	try
	{
		gmk.run(p1_type, p2_type, depth);
	} catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}
	return (0);
}
