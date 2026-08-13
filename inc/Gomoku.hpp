#pragma once

#include "platform/Window.hpp"

class	Gomoku
{
	#define TILES 19
	#define TILE_SIZE 32
	#define PIECE_SIZE 22
	#define WINDOW_SIZE (TILES * TILE_SIZE)
	#define WINDOW_TITLE "Gomoku"
	public:
		void	run()
		{
			init();
			loop();
		}
	private:
		void	init();
		void	loop();

		void	drawTile(int x, int y, int r, int g, int b);
		void	drawPiece(int x, int y, int r, int g, int b);
		void	renderOutline(int x, int y, int r, int g, int b);
		void	renderBoardBackground();
	private:
		Window	win;
		bool	running = false;
};
