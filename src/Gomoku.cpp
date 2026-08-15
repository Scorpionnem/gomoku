#include "Gomoku.hpp"
#include "AI.hpp"

bool humanPlayer = true;

void	Gomoku::init()
{
	win.open(WINDOW_TITLE, WINDOW_SIZE_X, WINDOW_SIZE_Y);
	AI::v.threads.add(16);
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
	static bool	AI_played = false;
	if (humanPlayer)
	{
		if (AI_played == false)
		{
			aiMove = AI::bestMove(game.getBoard(), game.getCurrentPlayer(), 3);
			AI_played = true;
		}

		if (input.wasPressed(SDL_BUTTON_LEFT))
		{
			AI_played = false;

			int	x = input.mouseX() / TILE_SIZE;
			int	y = input.mouseY() / TILE_SIZE;
			
			if (x >= BOARD_SIZE || y >= BOARD_SIZE || x < 0 || y < 0)
				return ;

			Position position = {x, y};
			Move playerMove = {position, game.getCurrentPlayer()};

			if (Move::isIllegalMove(game.getBoard(), playerMove))
			{
				std::cout << "Invalid / illegal move" << std::endl;
				return ;
			}
			playMove(playerMove);
			humanPlayer = false;
		}
	}
	if (humanPlayer)
	{
		if (AI_played == false)
		{
			aiMove = AI::bestMove(game.getBoard(), game.getCurrentPlayer(), 20);
			AI_played = true;
		}

		// if (input.wasPressed(SDLK_TAB))
		{
			AI_played = false;
			playMove(aiMove);	
			humanPlayer = true;
			turn++;
		}
	}
	(void)input;
}

void	Gomoku::playMove(Move move)
{
    game.getBoard().applyMove(move, game.getOpponent());

    if (game.getBoard().isWin(BLACK)
		|| game.getBoard().isWin(WHITE))
        return ;

    game.setCurrentPlayer(game.getOpponent());
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
	int	big = 0;
	for (auto &m : AI::v.ai_moves)
		if (m.score > big)
			big = m.score;
	for (auto &m : AI::v.ai_moves)
		drawPiece(m.m.getPosition(), Color{(int)(((float)m.score / (float)big) * 255), 255, 0});

	drawPiece(aiMove.getPosition(), Color{0, 0, 255});
	
	int	x = input.mouseX() / TILE_SIZE;
	int	y = input.mouseY() / TILE_SIZE;
	for (auto &m : AI::v.ai_moves)
	{
		if (m.m.getPosition().x == x && m.m.getPosition().y == y)
			win.drawText("score: " + std::to_string(m.score), input.mouseX(), input.mouseY());
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

	SDL_RenderPresent(win.getRenderer());

	renderOutline({input.mouseX() / TILE_SIZE, input.mouseY() / TILE_SIZE}, BLACK_COLOR);
	if (input.wasPressed(SDL_BUTTON_LEFT))
		renderOutline({input.mouseX() / TILE_SIZE, input.mouseY() / TILE_SIZE}, GREEN_COLOR);	

	win.drawText("time: " + std::to_string(AI::v.time) + "s", 9, WIN_BOARD_SIZE + 14);
	win.drawText("depth: " + std::to_string(AI::v.max_depth_explored), 9, WIN_BOARD_SIZE + 14 + 24);
	win.drawText("nodes: " + std::to_string(AI::v.explored_nodes.load()), 9, WIN_BOARD_SIZE + 14 + 24 + 24);
	win.drawText("turn: " + std::to_string(turn), 309, WIN_BOARD_SIZE + 14 + 24 + 24);
	win.drawText("bco: " + std::to_string(AI::v.branches_cut_off.load()), 309, WIN_BOARD_SIZE + 14);
	win.drawText("bre " + std::to_string(AI::v.branches_reach_end.load()), 309, WIN_BOARD_SIZE + 14 + 24);

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
