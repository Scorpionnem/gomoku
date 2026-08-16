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

		if (state == State::GAME)
		{
			updateGame(input);
		}
		else if (state == State::MENU) {}

		win.swapBuffers();
		SDL_Delay(16);
	}
}

void		Gomoku::getAction(Input &input)
{
	AI&			ai = player_turn == PLAYER1 ? ai_1 : ai_2;
	PlayerType	player_type = player_turn == PLAYER1 ? player1_type : player2_type;

	// does the player play this frame
	bool		play_frame = false;
	static Move	move;

	if (player_type == HUMANPLAYER)
	{
		static bool	compute_ai_move = false;

		if (compute_ai_move == false)
		{
			move = ai.bestMove(game.getBoard(), game.getCurrentPlayer(), 10);
			compute_ai_move = true;
		}

		if (input.wasPressed(SDL_BUTTON_LEFT))
		{
			int	x = input.mouseX() / TILE_SIZE;
			int	y = input.mouseY() / TILE_SIZE;

			if (x >= BOARD_SIZE || y >= BOARD_SIZE || x < 0 || y < 0)
				return ;

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
		move = ai.bestMove(game.getBoard(), game.getCurrentPlayer(), 10);
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
    if (game.getBoard().isWin(BLACK)
		|| game.getBoard().isWin(WHITE))
        return ;

    game.getBoard().applyMove(move, game.getOpponent());

    game.setCurrentPlayer(game.getOpponent());
}

void	Gomoku::drawAIDebug(AI &ai, Input &input)
{
	win.drawPiece(17 * TILE_SIZE, 20 * TILE_SIZE, (int)player_turn);

	auto	predicted_moves = ai.getEvaluatedMoves();
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
		drawPiece(m.second.m.getPosition(), Color{0, 0, (int)(f * 255)});
	}

	renderOutline(ai.getFinalMove().getPosition(), {0, 0, 255});

	int	x = input.mouseX() / TILE_SIZE;
	int	y = input.mouseY() / TILE_SIZE;
	for (auto &m : predicted_moves)
	{
		if (m.second.m.getPosition().x == x && m.second.m.getPosition().y == y)
			win.drawText("score: " + std::to_string(m.second.score), input.mouseX(), input.mouseY());
	}
}

void	Gomoku::updateGame(Input &input)
{
	win.drawBoard();

	Move moveInstance;
	auto illegalMoves = moveInstance.getIllegalMoves(
		game.getBoard(),
		game.getCurrentPlayer()
	);
	for (Move& move : illegalMoves)
		drawPiece(move.getPosition(), RED_COLOR);

	if (player_turn == PLAYER1)
	{
		drawAIDebug(ai_1, input);
	}
	else
	{
		drawAIDebug(ai_2, input);
	}

	for (int x = 0; x < BOARD_SIZE; x++)
		for (int y = 0; y < BOARD_SIZE; y++)
		{
			Position position = {x, y};
			if (game.getBoard().getPiece(position) == Piece::BLACK)
				win.drawPiece(position.x * TILE_SIZE, position.y * TILE_SIZE, 0);
			else if (game.getBoard().getPiece(position) == Piece::WHITE)
				win.drawPiece(position.x * TILE_SIZE, position.y * TILE_SIZE, 1);
		}

	renderOutline({input.mouseX() / TILE_SIZE, input.mouseY() / TILE_SIZE}, BLACK_COLOR);
	if (input.wasPressed(SDL_BUTTON_LEFT))
		renderOutline({input.mouseX() / TILE_SIZE, input.mouseY() / TILE_SIZE}, GREEN_COLOR);	

	const AI::Stats	&stats = ai_1.getStats();
	win.drawText("time: " + std::to_string(stats.time) + "s", 9, WIN_BOARD_SIZE + 14);
	win.drawText("depth: " + std::to_string(stats.max_depth), 9, WIN_BOARD_SIZE + 14 + 24);
	win.drawText("nodes: " + std::to_string(stats.explored_nodes), 9, WIN_BOARD_SIZE + 14 + 24 + 24);
	win.drawText("turn: " + std::to_string(turn), 309, WIN_BOARD_SIZE + 14 + 24 + 24);
	win.drawText("sn: " + std::to_string(stats.stopped_nodes), 309, WIN_BOARD_SIZE + 14);
	win.drawText("bdn " + std::to_string(stats.max_depth_nodes), 309, WIN_BOARD_SIZE + 14 + 24);

	if (game.getBoard().isWin(WHITE)
		|| game.getBoard().isWin(BLACK))
	{
		Piece	winner = game.getBoard().isWin(WHITE) ? WHITE : BLACK;
		std::string winner_str = winner == BLACK ? "black" : "white";

		win.drawText(winner_str + " wins", 0, 0, 255, 255, 0);

		return ;
	}
	else
	{
		getAction(input);
	
		// TEMPORARY INPUT FOR DEBUGGING
		if (input.wasPressed(SDLK_SPACE))
		{
			game.getBoard().undo();
			game.setCurrentPlayer(game.getOpponent());
		}
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

void	Gomoku::renderBoardBackground()
{
	for (int x = 0; x < TILES; x++)
		for (int y = 0; y < TILES; y++)
		{
			if ((x + y) % 2)
				drawTile({x, y}, BROWN_COLOR);
			else
				drawTile({x, y}, BEIGE_COLOR);
		}
	drawTile({9, 9}, DARK_BROWN_COLOR);
}
