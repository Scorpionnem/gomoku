#include "Gomoku.hpp"
#include "AI.hpp"

bool humanPlayer = true;

void	Gomoku::init()
{
	win.open(WINDOW_TITLE, WINDOW_SIZE);
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
		else if (state == State::MENU)
			;

		win.swapBuffers();
		SDL_Delay(16);
	}
}

void		Gomoku::getAction(Input &input)
{
	if (humanPlayer)
	{
		if (input.wasPressed(SDL_BUTTON_LEFT))
		{
			int	x = input.mouseX() / TILE_SIZE;
			int	y = input.mouseY() / TILE_SIZE;
			
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
	else if (!humanPlayer) {
		Move aiMove = AI::bestMove(game.getBoard(), game.getCurrentPlayer(), 3);
		playMove(aiMove);	
		humanPlayer = true;
	}
}

void	Gomoku::playMove(Move move)
{
    game.getBoard().applyMove(move, game.opponent());

    if (game.getBoard().isWin(game.getCurrentPlayer()))
	{
		std::cout << Game::toString(game.getCurrentPlayer()) << " wins" << std::endl;
        return ;
    }

    if (game.getBoard().isWin(game.opponent()))
	{
		std::cout << Game::toString(game.opponent()) << " wins" << std::endl;
        return ;
    }

    game.setCurrentPlayer(game.opponent());
}

void	Gomoku::updateGame(Input &input)
{
	win.drawBoard();

	getAction(input);

	// TEMPORARY INPUT FOR DEBUGGING
	if (input.wasPressed(SDLK_SPACE))
	{
		game.getBoard().undo();
		game.setCurrentPlayer(game.opponent());
	}

	Move moveInstance;
	auto illegalMoves = moveInstance.getIllegalMoves(
		game.getBoard(),
		game.getCurrentPlayer()
	);
	for (Move& move : illegalMoves)
		drawPiece(move.getPosition(), RED_COLOR);

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
