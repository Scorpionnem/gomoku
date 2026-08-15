#pragma once

# include "platform/Window.hpp"
# include "Game.hpp"
# include "Move.hpp"

# define BLACK_COLOR Color{0, 0, 0}
# define WHITE_COLOR Color{255, 255, 255}
# define RED_COLOR Color{255, 0, 0}
# define GREEN_COLOR Color{0, 255, 0}
# define BROWN_COLOR Color{230, 167, 80}
# define BEIGE_COLOR Color{204, 141, 53}
# define DARK_BROWN_COLOR Color{140, 90, 20}

struct Color {
	int r = 0;
	int g = 0;
	int b = 0;
};

class	Gomoku
{
	#define TILES 19
	#define TILE_SIZE 32
	#define PIECE_SIZE 22
	#define WIN_BOARD_SIZE (TILES * TILE_SIZE)
	#define WINDOW_SIZE_X (TILES * TILE_SIZE)
	#define WINDOW_SIZE_Y (TILES * TILE_SIZE + TILE_SIZE * 3)
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
		void	getAction(Input &input);
		void	playMove(Move move);

		void	drawTile(Position position, Color color);
		void	drawPiece(Position position, Color color);
		void	renderOutline(Position position, Color color);
		void	renderBoardBackground();
	private:
		Game	game;

		Window	win;
		bool	running = false;

		State	state = State::GAME;
};
