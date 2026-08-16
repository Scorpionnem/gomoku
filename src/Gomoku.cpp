#include "Gomoku.hpp"
#include "AI.hpp"

void	Gomoku::init()
{
	win.open(WINDOW_TITLE, WINDOW_SIZE_X, WINDOW_SIZE_Y);
}

void	Gomoku::loop()
{
	Input	input;

	running = true;

	while (running)
	{
		if (!win.pollEvents(input) || input.isDown(SDLK_ESCAPE))
		{
			running = false;
		}

		switch (state)
		{
			case State::GAME:
				updateGame(input);
				renderGame();
				break ;
			case State::GAME_DONE:
				updateGameDone(input);
				renderGameDone();
				break ;
			case State::MENU:
				updateMenu(input);
				renderMenu();
				break ;
		}

		win.swapBuffers();
		SDL_Delay(16);
	}
}

void	Gomoku::renderGameDone()
{
	win.drawBoard();
	drawPieces();

	drawPlayerPanel(ai_1, PLAYER1, 0);
	drawPlayerPanel(ai_2, PLAYER2, WIN_LEFT_OFFSET_PIXELS + WIN_BOARD_SIZE);

	std::string	winner_str = (winner == PLAYER1 ? "Player 1 wins!" : "Player 2 wins!");
	drawCenteredText(winner_str, WINDOW_SIZE_X / 2, WIN_BOARD_SIZE / 2 - 60);

	SDL_Rect	play_again = {WINDOW_SIZE_X / 2 - 220 / 2, 380, 220, 60};
	SDL_Rect	main_menu = {WINDOW_SIZE_X / 2 - 220 / 2, 460, 220, 60};

	drawButton(play_again, "Play Again", false);
	drawButton(main_menu, "Main Menu", false);
}

void	Gomoku::updateGameDone(Input &input)
{
	mouse_pos = {input.mouseX(), input.mouseY()};
	mouse_clicked = input.wasPressed(SDL_BUTTON_LEFT);

	if (!mouse_clicked)
		return ;

	SDL_Rect	play_again = {WINDOW_SIZE_X / 2 - 220 / 2, 380, 220, 60};
	SDL_Rect	main_menu = {WINDOW_SIZE_X / 2 - 220 / 2, 460, 220, 60};

	if (buttonClicked(play_again))
	{
		resetGame();
		state = State::GAME;
	}
	else if (buttonClicked(main_menu))
	{
		resetGame();
		state = State::MENU;
	}
}

void	Gomoku::renderMenu()
{
	drawCenteredText("Moku est un village de la commune de Pala du Comte de Jogeva en Estonie", WINDOW_SIZE_X / 2, 80);

	SDL_Rect	p1_human = {140, 260, 160, 50};
	SDL_Rect	p1_ai = {140, 260 + 50 + 20, 160, 50};
	SDL_Rect	p2_human = {WINDOW_SIZE_X - 140 - 160, 260, 160, 50};
	SDL_Rect	p2_ai = {WINDOW_SIZE_X - 140 - 160, 260 + 50 + 20, 160, 50};
	SDL_Rect	start = {WINDOW_SIZE_X / 2 - 220 / 2, 480, 220, 60};

	drawCenteredText("Player 1", p1_human.x + p1_human.w / 2, p1_human.y - 40);
	drawButton(p1_human, "Human", menu_p1_type == HUMANPLAYER);
	drawButton(p1_ai, "AI", menu_p1_type == AIPLAYER);

	drawCenteredText("Player 2", p2_human.x + p2_human.w / 2, p2_human.y - 40);
	drawButton(p2_human, "Human", menu_p2_type == HUMANPLAYER);
	drawButton(p2_ai, "AI", menu_p2_type == AIPLAYER);

	drawButton(start, "Start", false);
}

void	Gomoku::updateMenu(Input &input)
{
	mouse_pos = {input.mouseX(), input.mouseY()};
	mouse_clicked = input.wasPressed(SDL_BUTTON_LEFT);

	if (!mouse_clicked)
		return ;

	SDL_Rect	p1_human = {140, 260, 160, 50};
	SDL_Rect	p1_ai = {140, 260 + 50 + 20, 160, 50};
	SDL_Rect	p2_human = {WINDOW_SIZE_X - 140 - 160, 260, 160, 50};
	SDL_Rect	p2_ai = {WINDOW_SIZE_X - 140 - 160, 260 + 50 + 20, 160, 50};
	SDL_Rect	start = {WINDOW_SIZE_X / 2 - 220 / 2, 480, 220, 60};

	if (buttonClicked(p1_human))
		menu_p1_type = HUMANPLAYER;
	else if (buttonClicked(p1_ai))
		menu_p1_type = AIPLAYER;
	else if (buttonClicked(p2_human))
		menu_p2_type = HUMANPLAYER;
	else if (buttonClicked(p2_ai))
		menu_p2_type = AIPLAYER;
	else if (buttonClicked(start))
	{
		player1_type = menu_p1_type;
		player2_type = menu_p2_type;
		resetGame();
		state = State::GAME;
	}
}

void	Gomoku::renderGame()
{
	win.drawBoard();

	AI	&active_ai = player_turn == PLAYER1 ? ai_1 : ai_2;
	drawAIInfo(active_ai, true);

	drawPieces();

	drawCursor();

	drawPlayerPanel(ai_1, PLAYER1, 0);
	drawPlayerPanel(ai_2, PLAYER2, WIN_LEFT_OFFSET_PIXELS + WIN_BOARD_SIZE);

	std::string	turn_str = "turn: " + std::to_string(turn);
	win.drawText(turn_str, WIN_LEFT_OFFSET_PIXELS + WIN_BOARD_SIZE / 2 - 30, WIN_BOARD_SIZE + 36);
}

void	Gomoku::updateGame(Input &input)
{
	mouse_pos = {input.mouseX(), input.mouseY()};
	mouse_clicked = input.wasPressed(SDL_BUTTON_LEFT);

	if (game.getBoard().isWin(WHITE) || game.getBoard().isWin(BLACK))
	{
		state = State::GAME_DONE;
		winner = game.getBoard().isWin(BLACK) ? PLAYER1 : PLAYER2;
		return ;
	}

	getNextMove(input);
}

void	Gomoku::drawCursor()
{
	int	rel_x = mouse_pos.x - WIN_LEFT_OFFSET_PIXELS;
	int	rel_y = mouse_pos.y;

	if (rel_x < 0 || rel_x >= WIN_BOARD_SIZE || rel_y < 0 || rel_y >= WIN_BOARD_SIZE)
		return ;

	Position	board_mouse_pos = {rel_x / TILE_SIZE + WIN_LEFT_OFFSET, rel_y / TILE_SIZE};
	renderOutline(board_mouse_pos, BLACK_COLOR);
	if (mouse_clicked)
		renderOutline(board_mouse_pos, GREEN_COLOR);
}

void	Gomoku::drawPieces()
{
	for (int x = 0; x < BOARD_SIZE; x++)
		for (int y = 0; y < BOARD_SIZE; y++)
		{
			Position position = {x, y};
			if (game.getBoard().getPiece(position) == Piece::BLACK)
				win.drawPiece(position.x * TILE_SIZE + WIN_LEFT_OFFSET_PIXELS, position.y * TILE_SIZE, 0);
			else if (game.getBoard().getPiece(position) == Piece::WHITE)
				win.drawPiece(position.x * TILE_SIZE + WIN_LEFT_OFFSET_PIXELS, position.y * TILE_SIZE, 1);
		}

	auto illegalMoves = Move::getIllegalMoves(
		game.getBoard(),
		game.getCurrentPlayer()
	);
	for (Move& move : illegalMoves)
	{
		Position pos = move.getPosition();
		drawPiece({pos.x + WIN_LEFT_OFFSET, pos.y}, RED_COLOR);
	}
}

void	Gomoku::drawTile(Position position, Color color)
{
	win.drawFillRect(position.x * TILE_SIZE, position.y * TILE_SIZE, TILE_SIZE, TILE_SIZE, color.r, color.g, color.b);
}

void	Gomoku::drawPiece(Position position, Color color)
{
	win.drawFillRect(position.x * TILE_SIZE + ((TILE_SIZE - PIECE_SIZE) / 2),
						position.y * TILE_SIZE + ((TILE_SIZE - PIECE_SIZE) / 2),
						PIECE_SIZE, PIECE_SIZE, color.r, color.g, color.b);
}

void	Gomoku::renderOutline(Position position, Color color)
{
	win.drawRect(position.x * TILE_SIZE, position.y * TILE_SIZE, TILE_SIZE, TILE_SIZE, color.r, color.g, color.b);
}

void		Gomoku::getNextMove(Input &input)
{
	AI&			ai = player_turn == PLAYER1 ? ai_1 : ai_2;
	PlayerType	player_type = player_turn == PLAYER1 ? player1_type : player2_type;

	// does the player play this frame
	bool	play_frame = false;
	Move	move;

	if (player_type == HUMANPLAYER)
	{
		if (compute_ai_move == false)
		{
			move = ai.bestMove(game.getBoard(), game.getCurrentPlayer(), 1);
			compute_ai_move = true;
		}

		if (input.wasPressed(SDL_BUTTON_LEFT))
		{
			int	rel_x = input.mouseX() - WIN_LEFT_OFFSET_PIXELS;
			int	rel_y = input.mouseY();

			if (rel_x < 0 || rel_x >= WIN_BOARD_SIZE || rel_y < 0 || rel_y >= WIN_BOARD_SIZE)
				return ;

			int	x = rel_x / TILE_SIZE;
			int	y = rel_y / TILE_SIZE;

			Position position = {x, y};
			move = {position, game.getCurrentPlayer()};

			if (Move::isIllegalMove(game.getBoard(), move))
				return ;
			
			play_frame = true;
			compute_ai_move = false;
		}
	}
	else if (player_type == AIPLAYER)
	{
		move = ai.bestMove(game.getBoard(), game.getCurrentPlayer(), 1);
		play_frame = true;
	}

	if (play_frame)
	{
		player_turn = player_turn == PLAYER1 ? PLAYER2 : PLAYER1;
		turn++;

		playMove(move);
	}
}

void	Gomoku::playMove(Move move)
{
    game.getBoard().applyMove(move, game.getOpponent());

    game.setCurrentPlayer(game.getOpponent());
}

void	Gomoku::drawAIInfo(AI &ai, bool debug)
{
	auto	predicted_moves = ai.getEvaluatedMoves();

	if (debug)
	{
		int	big = INT_MIN;
		int	small = INT_MAX;
		for (auto &m : predicted_moves)
		{
			small = std::min(m.second.score, small);
			big = std::max(m.second.score, big);
		}
		for (auto &m : predicted_moves)
		{
			float	f = (float)(m.second.score - small) / (float)(big - small);
			Position	pos = m.second.m.getPosition();
			drawPiece({pos.x + WIN_LEFT_OFFSET, pos.y}, Color{0, 0, (int)(f * 255)});
		}
	}

	Position pos = ai.getFinalMove().getPosition();
	renderOutline({pos.x + WIN_LEFT_OFFSET, pos.y}, {0, 0, 255});

	if (debug)
	{
		int	rel_x = mouse_pos.x - WIN_LEFT_OFFSET_PIXELS;
		int	rel_y = mouse_pos.y;

		if (rel_x >= 0 && rel_x < WIN_BOARD_SIZE && rel_y >= 0 && rel_y < WIN_BOARD_SIZE)
		{
			Position	board_mouse_pos = {rel_x / TILE_SIZE, rel_y / TILE_SIZE};

			for (auto &m : predicted_moves)
			{
				pos = m.second.m.getPosition();
				if (pos == board_mouse_pos)
					win.drawText("score: " + std::to_string(m.second.score), mouse_pos.x, mouse_pos.y - 20);
			}
		}
	}
}

void	Gomoku::drawPlayerPanel(AI &ai, PlayerTurn player, int panel_x)
{
	bool	is_turn = (player_turn == player);
	int	piece_index = (player == PLAYER1) ? 0 : 1;
	int	indicator_x = panel_x + (WIN_LEFT_OFFSET_PIXELS - TILE_SIZE) / 2;
	int	indicator_y = 16;

	Piece topiece = player == PlayerTurn::PLAYER1 ? BLACK : WHITE;

	win.drawPiece(indicator_x, indicator_y, piece_index);
	if (is_turn)
		win.drawRect(indicator_x, indicator_y, TILE_SIZE, TILE_SIZE, 0, 255, 0);

	win.drawText(player == PLAYER1 ? "Player 1" : "Player 2", panel_x + 10, indicator_y + TILE_SIZE + 14);

	const AI::Stats	&stats = ai.getStats();
	int	y = indicator_y + TILE_SIZE + 50;

	win.drawText("time: " + std::to_string(stats.time) + "s", panel_x + 10, y);
	win.drawText("depth: " + std::to_string(stats.max_depth), panel_x + 10, y + 24);
	win.drawText("nodes: " + std::to_string(stats.explored_nodes), panel_x + 10, y + 24 * 2);
	win.drawText("stop: " + std::to_string(stats.stopped_nodes), panel_x + 10, y + 24 * 3);
	win.drawText("maxdn: " + std::to_string(stats.max_depth_nodes), panel_x + 10, y + 24 * 4);
	win.drawText("capture: " + std::to_string(game.getBoard().getCaptureCount(topiece)), panel_x + 10, y + 24 * 5);
}

void	Gomoku::resetGame()
{
	game = Game();
	ai_1 = AI();
	ai_2 = AI();
	player_turn = PLAYER1;
	turn = 0;
	compute_ai_move = false;
}

bool	Gomoku::isInside(SDL_Rect r, int px, int py) const
{
	return (px >= r.x && px < r.x + r.w && py >= r.y && py < r.y + r.h);
}

bool	Gomoku::buttonClicked(SDL_Rect r) const
{
	return (mouse_clicked && isInside(r, mouse_pos.x, mouse_pos.y));
}

void	Gomoku::drawButton(SDL_Rect r, const std::string &label, bool selected)
{
	bool	hovered = isInside(r, mouse_pos.x, mouse_pos.y);
	Color	bg = selected ? Color{70, 150, 70} : (hovered ? Color{90, 90, 90} : Color{45, 45, 45});

	win.drawFillRect(r.x, r.y, r.w, r.h, bg.r, bg.g, bg.b);
	win.drawRect(r.x, r.y, r.w, r.h, 255, 255, 255);

	int	tw;
	int	th;
	win.getTextSize(label, tw, th);
	win.drawText(label, r.x + (r.w - tw) / 2, r.y + (r.h - th) / 2);
}

void	Gomoku::drawCenteredText(const std::string &s, int cx, int y)
{
	int	tw;
	int	th;
	win.getTextSize(s, tw, th);
	win.drawText(s, cx - tw / 2, y);
}
