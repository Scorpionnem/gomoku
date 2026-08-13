#pragma once

#include "platform/Window.hpp"

#include <set>

class	Gomoku
{
	#define TILES 19
	#define TILE_SIZE 32
	#define PIECE_SIZE 22
	#define WINDOW_SIZE (TILES * TILE_SIZE)
	#define WINDOW_TITLE "Gomoku"
	enum Player
	{
		AIPLAYER,
		HUMANPLAYER,
	};
	enum State
	{
		MENU,
		GAME,
	};
	enum PlayerTurn
	{
		P1TURN,
		P2TURN,
	};
	public:
		void	run()
		{
			init();
			loop();
		}
	private:
		void	init();
		void	loop();

		void	game(Input &input);
		u32		getAction(Input &input, Gomoku::Player p);

		void	drawTile(int x, int y, int r, int g, int b);
		void	drawPiece(int x, int y, int r, int g, int b);
		void	renderOutline(int x, int y, int r, int g, int b);
		void	renderBoardBackground();
	private:
		Window	win;
		bool	running = false;

		State	state = State::GAME;

		Player	p1 = Player::HUMANPLAYER;
		Player	p2 = Player::HUMANPLAYER;
		PlayerTurn	pTurn = P1TURN;

		std::set<u32>	plays; // test
};
