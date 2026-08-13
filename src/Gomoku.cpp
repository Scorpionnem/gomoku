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
			
			playMove(x, y);
		}
	}
	else if (p == AIPLAYER) {}
}

void	Gomoku::playMove(int x, int y)
{
    Move playerMove = {x, y, game.getCurrentPlayer()};

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
		std::cout << (game.getCurrentPlayer() == BLACK ? "Red" : "Blue") << " wins" << std::endl;
        return ;
    }

    if (game.getBoard().isWin(game.opponent()))
	{
		std::cout << (game.opponent() == BLACK ? "Red" : "Blue") << " wins" << std::endl;
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

	Piece (*b)[BOARD_SIZE] = game.getBoard().getBoard();
	
	Move moveInstance;
	auto illegalMoves = moveInstance.getIllegalMoves(
		game.getBoard(),
		game.getCurrentPlayer()
	);
	for (Move& move : illegalMoves)
		drawPiece(move.getX(), move.getY(), 255, 0, 0);

	for (int x = 0; x < BOARD_SIZE; x++)
		for (int y = 0; y < BOARD_SIZE; y++)
		{
			if (b[x][y] == Piece::BLACK)
				drawPiece(x, y, 0, 0, 0);
			else if (b[x][y] == Piece::WHITE)
				drawPiece(x, y, 255, 255, 255);
		}

	renderOutline(input.mouseX() / TILE_SIZE, input.mouseY() / TILE_SIZE, 0, 0, 0);
	if (input.wasPressed(SDL_BUTTON_LEFT))
		renderOutline(input.mouseX() / TILE_SIZE, input.mouseY() / TILE_SIZE, 0, 255, 0);
}

void	Gomoku::drawTile(int x, int y, int r, int g, int b)
{
	win.drawFillRect(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, r, g, b);
}

void	Gomoku::drawPiece(int x, int y, int r, int g, int b)
{
	win.drawFillRect(x * TILE_SIZE + ((TILE_SIZE - PIECE_SIZE) / 2),
						y * TILE_SIZE + ((TILE_SIZE - PIECE_SIZE) / 2),
						PIECE_SIZE, PIECE_SIZE, r, g, b);
}

void	Gomoku::renderOutline(int x, int y, int r, int g, int b)
{
	win.drawRect(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, r, g, b);
}

void	Gomoku::renderBoardBackground()
{
	for (int x = 0; x < TILES; x++)
		for (int y = 0; y < TILES; y++)
		{
			if ((x + y) % 2)
				drawTile(x, y, 230, 167, 80);
			else
				drawTile(x, y, 204, 141, 53);
		}
	drawTile(9, 9, 140, 90, 20);
}
