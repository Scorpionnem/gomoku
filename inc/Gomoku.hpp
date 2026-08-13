#pragma once

#include "platform/Window.hpp"
#include "Game.hpp"

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
	public:
		void	run()
		{
			init();
			loop();
		}
	private:
		void	init();
		void	loop();

		void	updateGame(Input &input);
		u32		getAction(Input &input, int p);
		void	playMove(int x, int y);

		void	drawTile(int x, int y, int r, int g, int b);
		void	drawPiece(int x, int y, int r, int g, int b);
		void	renderOutline(int x, int y, int r, int g, int b);
		void	renderBoardBackground();
	private:
		Game	game;

		Window	win;
		bool	running = false;

		State	state = State::GAME;
};
