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

	drawPlayerPanel(ai_1, BLACK, 0);
	drawPlayerPanel(ai_2, WHITE, WIN_LEFT_OFFSET_PIXELS + WIN_BOARD_SIZE);

	std::string	winner_str = (winner == BLACK ? "Player 1 wins!" : "Player 2 wins!");
	drawCenteredText(winner_str, WINDOW_SIZE_X / 2, WIN_BOARD_SIZE / 2 - 60);
	
	drawCenteredText(std::to_string(turn) + " turns", WINDOW_SIZE_X / 2, WIN_BOARD_SIZE / 2);

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

struct	PlayerColumnRects
{
	SDL_Rect	human;
	SDL_Rect	ai;
	SDL_Rect	debug;
	SDL_Rect	hint;
	SDL_Rect	colors[NUM_PIECE_COLORS];
};

PlayerColumnRects	getPlayerColumnRects(int col_x)
{
	PlayerColumnRects	r;
	int	button_w = 160;
	int	button_h = 40;
	int	gap = 8;

	r.human = {col_x, 240, button_w, button_h};
	r.ai = {col_x, 240 + (button_h + gap), button_w, button_h};
	r.debug = {col_x, 240 + 2 * (button_h + gap), button_w, button_h};
	r.hint = {col_x, 240 + 3 * (button_h + gap), button_w, button_h};

	int	picker = 36;
	int	picker_gap = 6;
	int	row1_y = 240 + 4 * (button_h + gap);
	int	row2_y = row1_y + picker + picker_gap;
	int	pad = (button_w - (picker * 3 + picker_gap * 2)) / 2;

	for (int i = 0; i < 3; i++)
		r.colors[i] = {col_x + pad + i * (picker + picker_gap), row1_y, picker, picker};
	for (int i = 0; i < 3; i++)
		r.colors[3 + i] = {col_x + pad + i * (picker + picker_gap), row2_y, picker, picker};

	return (r);
}

static SDL_Rect	getStartButtonRect()
{
	const int	w = 220;
	const int	h = 60;

	return (SDL_Rect{WINDOW_SIZE_X / 2 - w / 2, 580, w, h});
}

struct	PanelToggleRects
{
	SDL_Rect	debug;
	SDL_Rect	hint;
};
static PanelToggleRects	getPanelToggleRects(int panel_x)
{
	int	y = 16 + TILE_SIZE + 42;
	int	h = 32;
	int	gap = 6;
	int	w = (WIN_LEFT_OFFSET_PIXELS - 20 - gap) / 2;
	return (PanelToggleRects{
		{panel_x + 10, y, w, h},
		{panel_x + 10 + w + gap, y, w, h},
	});
}

void	Gomoku::renderMenu()
{
	win.drawMewen();
	
	drawCenteredText(splash_texts[splash_text], WINDOW_SIZE_X / 2, 80);

	PlayerColumnRects	col1 = getPlayerColumnRects(140);
	PlayerColumnRects	col2 = getPlayerColumnRects(WINDOW_SIZE_X - 140 - 160);
	SDL_Rect		start = getStartButtonRect();

	drawCenteredText("Player 1", col1.human.x + col1.human.w / 2, col1.human.y - 40);
	drawButton(col1.human, "Human", menu_p1_type == HUMANPLAYER);
	drawButton(col1.ai, "AI", menu_p1_type == AIPLAYER);
	drawButton(col1.debug, menu_p1_debug ? "Debug: ON" : "Debug: OFF", menu_p1_debug);
	drawButton(col1.hint, menu_p1_hint ? "Hint: ON" : "Hint: OFF", menu_p1_hint);
	for (int i = 0; i < NUM_PIECE_COLORS; i++)
		drawColorPicker(col1.colors[i], PIECE_STYLES[i].color, menu_p1_color_idx == i);
	drawCenteredText(PIECE_STYLES[menu_p1_color_idx].name, col1.human.x + col1.human.w / 2, col1.colors[3].y + col1.colors[3].h + 8);

	drawCenteredText("Player 2", col2.human.x + col2.human.w / 2, col2.human.y - 40);
	drawButton(col2.human, "Human", menu_p2_type == HUMANPLAYER);
	drawButton(col2.ai, "AI", menu_p2_type == AIPLAYER);
	drawButton(col2.debug, menu_p2_debug ? "Debug: ON" : "Debug: OFF", menu_p2_debug);
	drawButton(col2.hint, menu_p2_hint ? "Hint: ON" : "Hint: OFF", menu_p2_hint);
	for (int i = 0; i < NUM_PIECE_COLORS; i++)
		drawColorPicker(col2.colors[i], PIECE_STYLES[i].color, menu_p2_color_idx == i);
	drawCenteredText(PIECE_STYLES[menu_p2_color_idx].name, col2.human.x + col2.human.w / 2, col2.colors[3].y + col2.colors[3].h + 8);

	drawButton(start, "Start", false);
}

void	Gomoku::updateMenu(Input &input)
{
	mouse_pos = {input.mouseX(), input.mouseY()};
	mouse_clicked = input.wasPressed(SDL_BUTTON_LEFT);

	if (!mouse_clicked)
		return ;

	PlayerColumnRects	col1 = getPlayerColumnRects(140);
	PlayerColumnRects	col2 = getPlayerColumnRects(WINDOW_SIZE_X - 140 - 160);
	SDL_Rect		start = getStartButtonRect();

	if (buttonClicked(col1.human))
		menu_p1_type = HUMANPLAYER;
	else if (buttonClicked(col1.ai))
		menu_p1_type = AIPLAYER;
	else if (buttonClicked(col1.debug))
		menu_p1_debug = !menu_p1_debug;
	else if (buttonClicked(col1.hint))
		menu_p1_hint = !menu_p1_hint;
	else if (buttonClicked(col2.human))
		menu_p2_type = HUMANPLAYER;
	else if (buttonClicked(col2.ai))
		menu_p2_type = AIPLAYER;
	else if (buttonClicked(col2.debug))
		menu_p2_debug = !menu_p2_debug;
	else if (buttonClicked(col2.hint))
		menu_p2_hint = !menu_p2_hint;
	else if (buttonClicked(start))
	{
		player1_type = menu_p1_type;
		player2_type = menu_p2_type;
		player1_debug = menu_p1_debug;
		player2_debug = menu_p2_debug;
		player1_hint = menu_p1_hint;
		player2_hint = menu_p2_hint;
		player1_color_idx = menu_p1_color_idx;
		player2_color_idx = menu_p2_color_idx;
		resetGame();
		state = State::GAME;
	}
	else
	{
		for (int i = 0; i < NUM_PIECE_COLORS; i++)
		{
			if (buttonClicked(col1.colors[i]) && menu_p2_color_idx != i)
				menu_p1_color_idx = i;
			else if (buttonClicked(col2.colors[i]) && menu_p1_color_idx != i)
				menu_p2_color_idx = i;
		}
	}
}

void	Gomoku::renderGame()
{
	win.drawBoard();

	AI	&active_ai = player_turn == BLACK ? ai_1 : ai_2;
	bool	active_debug = player_turn == BLACK ? player1_debug : player2_debug;
	bool	active_hint = player_turn == BLACK ? player1_hint : player2_hint;
	drawAIInfo(active_ai, active_debug, active_hint);

	drawPieces();

	drawCursor();

	drawPlayerPanel(ai_1, BLACK, 0);
	drawPlayerPanel(ai_2, WHITE, WIN_LEFT_OFFSET_PIXELS + WIN_BOARD_SIZE);

	std::string	turn_str = "turn: " + std::to_string(turn);
	win.drawText(turn_str, WIN_LEFT_OFFSET_PIXELS + WIN_BOARD_SIZE / 2 - 30, WIN_BOARD_SIZE + 36);
}

void	Gomoku::updateGame(Input &input)
{
	mouse_pos = {input.mouseX(), input.mouseY()};
	mouse_clicked = input.wasPressed(SDL_BUTTON_LEFT);

	if (getBoard().isWin(WHITE) || board.isWin(BLACK))
	{
		state = State::GAME_DONE;
		winner = board.isWin(BLACK) ? BLACK : WHITE;
		return ;
	}

	if (mouse_clicked)
	{
		PanelToggleRects	p1 = getPanelToggleRects(0);
		PanelToggleRects	p2 = getPanelToggleRects(WIN_LEFT_OFFSET_PIXELS + WIN_BOARD_SIZE);
		if (buttonClicked(p1.debug))
		{
			player1_debug = !player1_debug;
			return ;
		}
		if (buttonClicked(p1.hint))
		{
			player1_hint = !player1_hint;
			return ;
		}
		if (buttonClicked(p2.debug))
		{
			player2_debug = !player2_debug;
			return ;
		}
		if (buttonClicked(p2.hint))
		{
			player2_hint = !player2_hint;
			return ;
		}
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
			if (board.getPiece(position) == Piece::BLACK)
				drawGamePiece(position.x * TILE_SIZE + WIN_LEFT_OFFSET_PIXELS, position.y * TILE_SIZE, player1_color_idx);
			else if (board.getPiece(position) == Piece::WHITE)
				drawGamePiece(position.x * TILE_SIZE + WIN_LEFT_OFFSET_PIXELS, position.y * TILE_SIZE, player2_color_idx);
		}

	auto illegalMoves = Move::getIllegalMoves(
		board,
		getCurrentPlayer()
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
	AI&			ai = player_turn == BLACK ? ai_1 : ai_2;
	PlayerType	player_type = player_turn == BLACK ? player1_type : player2_type;
	std::vector<float>	&times = player_turn == BLACK ? p1_times : p2_times;

	// does the player play this frame
	bool	play_frame = false;
	Move	move;

	if (player1_type == AIPLAYER && player2_type == AIPLAYER && first_place_ai_ai == false)
	{
		first_place_ai_ai = true;

		int	x = rand() % 19;
		int	y = rand() % 19;

		Position position = {x, y};
		move = {position, getCurrentPlayer()};
		play_frame = true;
	}
	else if (player_type == HUMANPLAYER)
	{
		bool	need_ai = (player_turn == BLACK)
			? (player1_debug || player1_hint)
			: (player2_debug || player2_hint);
		if (need_ai && compute_ai_move == false)		{
			move = ai.bestMove(board, getCurrentPlayer(), 10);
			times.push_back(ai.getStats().time);
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
			move = {position, getCurrentPlayer()};

			if (Move::isIllegalMove(board, move))
				return ;
			
			play_frame = true;
			compute_ai_move = false;
		}
	}
	else if (player_type == AIPLAYER)
	{
		move = ai.bestMove(board, getCurrentPlayer(), 10);
		times.push_back(ai.getStats().time);
		play_frame = true;
	}

	if (play_frame)
	{
		playMove(move);
		if (player_turn == WHITE)
			turn++;
	}
}

void Gomoku::playMove(Move move)
{
	board.applyMove(move, getOpponent(move.getPiece()));
	setCurrentPlayer(getOpponent(getCurrentPlayer()));
}

void	Gomoku::drawAIInfo(AI &ai, bool debug, bool hint)
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

	if (hint)
	{
		Position pos = ai.getFinalMove().getPosition();
		renderOutline({pos.x + WIN_LEFT_OFFSET, pos.y}, {0, 0, 255});
	}

	if (debug)
	{
		int	rel_x = mouse_pos.x - WIN_LEFT_OFFSET_PIXELS;
		int	rel_y = mouse_pos.y;

		if (rel_x >= 0 && rel_x < WIN_BOARD_SIZE && rel_y >= 0 && rel_y < WIN_BOARD_SIZE)
		{
			Position	board_mouse_pos = {rel_x / TILE_SIZE, rel_y / TILE_SIZE};

			for (auto &m : predicted_moves)
			{
				Position pos = m.second.m.getPosition();
				if (pos == board_mouse_pos)
					win.drawText("score: " + std::to_string(m.second.score), mouse_pos.x, mouse_pos.y - 20);
			}
		}
	}
}

void	Gomoku::drawPlayerPanel(AI &ai, Piece player, int panel_x)
{
	bool	is_turn = (player_turn == player);

	int	color_idx = (player == BLACK) ? player1_color_idx : player2_color_idx;
	std::vector<float>	&times = player == BLACK ? p1_times : p2_times;

	int	indicator_x = panel_x + (WIN_LEFT_OFFSET_PIXELS - TILE_SIZE) / 2;
	bool	debug = (player == BLACK) ? player1_debug : player2_debug;
	bool	hint = (player == BLACK) ? player1_hint : player2_hint;
	int	indicator_y = 16;

	drawGamePiece(indicator_x, indicator_y, color_idx);
	if (is_turn)
		win.drawRect(indicator_x, indicator_y, TILE_SIZE, TILE_SIZE, 0, 255, 0);

	win.drawText(player == BLACK ? "Player 1" : "Player 2", panel_x + 10, indicator_y + TILE_SIZE + 14);


	PanelToggleRects	toggles = getPanelToggleRects(panel_x);
	drawButton(toggles.debug, "Debug", debug);
	drawButton(toggles.hint, "Hint", hint);

	const AI::Stats	&stats = ai.getStats();
	int	y = toggles.hint.y + toggles.hint.h + 12;

	float	avg = 0;
	for (float f : times)
		avg += f;
	if (!times.empty())
		avg /= times.size();

	win.drawText("time: " + std::to_string(stats.time) + "s", panel_x + 10, y);
	if (debug)
	{
		win.drawText("avg: " + std::to_string(avg) + "s", panel_x + 10, y + 24);
		win.drawText("depth: " + std::to_string(stats.max_depth), panel_x + 10, y + 24 * 2);
		win.drawText("nodes: " + std::to_string(stats.explored_nodes), panel_x + 10, y + 24 * 3);
		win.drawText("stop: " + std::to_string(stats.stopped_nodes), panel_x + 10, y + 24 * 4);
		win.drawText("maxdn: " + std::to_string(stats.max_depth_nodes), panel_x + 10, y + 24 * 5);
	}
	win.drawText("capture: " + std::to_string(board.getCaptureCount(player)), panel_x + 10, y + 24 * (debug ? 6 : 1));
}

void	Gomoku::drawGamePiece(int x, int y, int color_idx)
{
	Color c = PIECE_STYLES[color_idx].color;
	win.drawPiece(x, y, c.r, c.g, c.b);
}

void	Gomoku::drawColorPicker(SDL_Rect r, Color c, bool selected)
{
	win.drawFillRect(r.x, r.y, r.w, r.h, c.r, c.g, c.b);
	if (selected)
		win.drawRect(r.x - 2, r.y - 2, r.w + 4, r.h + 4, 0, 255, 0);
	else
		win.drawRect(r.x, r.y, r.w, r.h, 255, 255, 255);
}

void	Gomoku::resetGame()
{
	first_place_ai_ai = false;
	board = Board();
	ai_1 = AI();
	ai_2 = AI();
	player_turn = BLACK;
	turn = 0;
	compute_ai_move = false;
	p1_times.clear();
	p2_times.clear();
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
