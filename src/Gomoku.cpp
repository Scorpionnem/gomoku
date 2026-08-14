#include "Gomoku.hpp"

#include <vector>

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

void		Gomoku::getAction(Input &input, int p)
{
	if (p == HUMANPLAYER)
	{
		if (input.wasPressed(SDL_BUTTON_LEFT))
		{
			int	x = input.mouseX() / TILE_SIZE;
			int	y = input.mouseY() / TILE_SIZE;
			
			playMove({x, y});
		}
	}
	else if (p == AIPLAYER) {}
}

void	Gomoku::playMove(Position position)
{
    Move playerMove = {position, game.getCurrentPlayer()};

    if (Move::isIllegalMove(game.getBoard(), playerMove))
	{
		std::cout << "Invalid / illegal move" << std::endl;
        return ;
    }

    game.getBoard().play(playerMove);

    CaptureInfo captureInfo = game.getBoard().findCaptures(
        playerMove,
        game.opponent()
    );

    if (captureInfo.capturedCount > 0)
	{
        playerMove.setType(CAPTURE);
        playerMove.setRemovedPositions(captureInfo.removedPositions);
        game.getBoard().setLastMove(playerMove);
        game.getBoard().incrementCaptureCount(
            game.getCurrentPlayer(),
            captureInfo.capturedCount
        );
        game.getBoard().applyCaptures(captureInfo);
    }

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
	renderBoardBackground();

	getAction(input, 1);

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
				drawPiece(position, BLACK_COLOR);
			else if (game.getBoard().getPiece(position) == Piece::WHITE)
				drawPiece(position, WHITE_COLOR);
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
