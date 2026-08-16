#pragma once

# include "platform/Window.hpp"
# include "Game.hpp"
# include "Move.hpp"
# include "AI.hpp"

# include <iostream>
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
	public:
		#define TILES 19
		#define TILE_SIZE 32
		#define PIECE_SIZE 26
		#define WIN_BOARD_SIZE (TILES * TILE_SIZE)
		#define WINDOW_SIZE_X (TILES * TILE_SIZE)
		#define WINDOW_SIZE_Y (TILES * TILE_SIZE + TILE_SIZE * 3)
		#define WINDOW_TITLE "Gomoku"
		enum PlayerType
		{
			AIPLAYER,
			HUMANPLAYER,
		};
		enum PlayerTurn
		{
			PLAYER1,
			PLAYER2,
		};
		enum State
		{
			MENU,
			GAME,
		};
	public:
		void	run(PlayerType p1_type = HUMANPLAYER, PlayerType p2_type = AIPLAYER)
		{
			player1_type = p1_type;
			player2_type = p2_type;

			init();
			loop();
		}
	private:
		void	init();
		void	loop();

		void	updateGame(Input &input);
		void	getAction(Input &input);
		void	playMove(Move move);

		void	drawAIDebug(AI &ai, Input &input);

		void	drawTile(Position position, Color color);
		void	drawPiece(Position position, Color color);
		void	renderOutline(Position position, Color color);
		void	renderBoardBackground();
	private:
		PlayerTurn	player_turn = PLAYER1;

		PlayerType	player1_type = AIPLAYER;
		PlayerType	player2_type = HUMANPLAYER;

		AI		ai_1;
		AI		ai_2;

		Game	game;

		Window	win;
		bool	running = false;

		State	state = State::GAME;
		int		turn = 0;
};
