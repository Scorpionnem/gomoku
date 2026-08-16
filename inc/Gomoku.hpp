#pragma once

# include "platform/Window.hpp"
# include "Game.hpp"
# include "Move.hpp"
# include "AI.hpp"

# include <iostream>
# define BLACK_COLOR Color{0, 0, 0}
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
		#define WIN_LEFT_OFFSET_PIXELS (6 * TILE_SIZE)
		#define WIN_LEFT_OFFSET (6)
		#define WINDOW_SIZE_X (TILES * TILE_SIZE + TILE_SIZE * 12)
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
			GAME_DONE,
		};
	public:
		void	run()
		{
			init();
			loop();
		}
		void	run(PlayerType p1_type, PlayerType p2_type)
		{
			state = State::GAME;
			player1_type = p1_type;
			player2_type = p2_type;
			init();
			loop();
		}
	private:
		void	init();
		void	loop();

		void	updateMenu(Input &input);
		void	renderMenu();
		
		void	updateGameDone(Input &input);
		void	renderGameDone();

		void	updateGame(Input &input);
		void	renderGame();

		void	playMove(Move move);
		void	getNextMove(Input &input);

		void	drawPieces();
		void	drawCursor();
		void	drawAIInfo(AI &ai, bool debug);
		void	drawPlayerPanel(AI &ai, PlayerTurn player, int panel_x);
		void	drawTile(Position position, Color color);
		void	drawPiece(Position position, Color color);
		void	renderOutline(Position position, Color color);

		void	resetGame();

		bool	isInside(SDL_Rect r, int px, int py) const;
		bool	buttonClicked(SDL_Rect r) const;
		void	drawButton(SDL_Rect r, const std::string &label, bool selected);
		void	drawCenteredText(const std::string &s, int cx, int y);
	private:
		PlayerTurn	player_turn = PLAYER1;

		PlayerType	player1_type = AIPLAYER;
		PlayerType	player2_type = HUMANPLAYER;
		int			depth = 10;

		PlayerType	menu_p1_type = HUMANPLAYER;
		PlayerType	menu_p2_type = AIPLAYER;

		PlayerTurn	winner = PLAYER1;

		AI		ai_1;
		AI		ai_2;

		Game	game;

		Window	win;
		bool	running = false;

		State	state = State::MENU;
		int		turn = 0;

		Position	mouse_pos = {};
		bool		mouse_clicked = false;

		bool	compute_ai_move = false;
};
